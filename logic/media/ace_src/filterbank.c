#include "ace_config.h"

#include "filterbank.h"
#include "ace_arch.h"
#include <math.h>
#include "math_approx.h"
#include "os_support.h"
      
#ifdef FIXED_POINT

#define toBARK(n)   (MULT16_16(26829,ace_atan(SHR32(MULT16_16(97,n),2))) + MULT16_16(4588,ace_atan(MULT16_32_Q15(20,MULT16_16(n,n)))) + MULT16_16(3355,n))
      
#else
#define toBARK(n)   (13.1f*atan(.00074f*(n))+2.24f*atan((n)*(n)*1.85e-8f)+1e-4f*(n))
#endif
       
#define toMEL(n)    (2595.f*log10(1.f+(n)/700.f))

FilterBank *filterbank_new(int banks, ace_word32_t sampling, int len, int type)
{
   FilterBank *bank;
   ace_word32_t df;
   ace_word32_t max_mel, mel_interval;
   int i;
   int id1;
   int id2;
   df = DIV32(SHL32(sampling,15),MULT16_16(2,len));
   max_mel = toBARK(EXTRACT16(sampling/2));
   mel_interval = PDIV32(max_mel,banks-1);
   
   bank = (FilterBank*)ace_alloc(sizeof(FilterBank));
   bank->nb_banks = banks;
   bank->len = len;
   bank->bank_left = (int*)ace_alloc(len*sizeof(int));
   bank->bank_right = (int*)ace_alloc(len*sizeof(int));
   bank->filter_left = (ace_word16_t*)ace_alloc(len*sizeof(ace_word16_t));
   bank->filter_right = (ace_word16_t*)ace_alloc(len*sizeof(ace_word16_t));
   /* Think I can safely disable normalisation that for fixed-point (and probably float as well) */
#ifndef FIXED_POINT
   bank->scaling = (float*)ace_alloc(banks*sizeof(float));
#endif
   for (i=0;i<len;i++)
   {
      ace_word16_t curr_freq;
      ace_word32_t mel;
      ace_word16_t val;
      curr_freq = EXTRACT16(MULT16_32_P15(i,df));
      mel = toBARK(curr_freq);
      if (mel > max_mel)
         break;
#ifdef FIXED_POINT
      id1 = DIV32(mel,mel_interval);
#else      
      id1 = (int)(floor(mel/mel_interval));
#endif
      if (id1>banks-2)
      {
         id1 = banks-2;
         val = Q15_ONE;
      } else {
         val = DIV32_16(mel - id1*mel_interval,EXTRACT16(PSHR32(mel_interval,15)));
      }
      id2 = id1+1;
      bank->bank_left[i] = id1;
      bank->filter_left[i] = SUB16(Q15_ONE,val);
      bank->bank_right[i] = id2;
      bank->filter_right[i] = val;
   }
   
   /* Think I can safely disable normalisation for fixed-point (and probably float as well) */
#ifndef FIXED_POINT
   for (i=0;i<bank->nb_banks;i++)
      bank->scaling[i] = 0;
   for (i=0;i<bank->len;i++)
   {
      int id = bank->bank_left[i];
      bank->scaling[id] += bank->filter_left[i];
      id = bank->bank_right[i];
      bank->scaling[id] += bank->filter_right[i];
   }
   for (i=0;i<bank->nb_banks;i++)
      bank->scaling[i] = Q15_ONE/(bank->scaling[i]);
#endif
   return bank;
}

void filterbank_destroy(FilterBank *bank)
{
   ace_free(bank->bank_left);
   ace_free(bank->bank_right);
   ace_free(bank->filter_left);
   ace_free(bank->filter_right);
#ifndef FIXED_POINT
   ace_free(bank->scaling);
#endif
   ace_free(bank);
}

void filterbank_compute_bank32(FilterBank *bank, ace_word32_t *ps, ace_word32_t *mel)
{
   int i;
   for (i=0;i<bank->nb_banks;i++)
      mel[i] = 0;

   for (i=0;i<bank->len;i++)
   {
      int id;
      id = bank->bank_left[i];
      mel[id] += MULT16_32_P15(bank->filter_left[i],ps[i]);
      id = bank->bank_right[i];
      mel[id] += MULT16_32_P15(bank->filter_right[i],ps[i]);
   }
   /* Think I can safely disable normalisation that for fixed-point (and probably float as well) */
#ifndef FIXED_POINT
   /*for (i=0;i<bank->nb_banks;i++)
      mel[i] = MULT16_32_P15(Q15(bank->scaling[i]),mel[i]);
   */
#endif
}

void filterbank_compute_psd16(FilterBank *bank, ace_word16_t *mel, ace_word16_t *ps)
{
   int i;
   for (i=0;i<bank->len;i++)
   {
      ace_word32_t tmp;
      int id1, id2;
      id1 = bank->bank_left[i];
      id2 = bank->bank_right[i];
      tmp = MULT16_16(mel[id1],bank->filter_left[i]);
      tmp += MULT16_16(mel[id2],bank->filter_right[i]);
      ps[i] = EXTRACT16(PSHR32(tmp,15));
   }
}


#ifndef FIXED_POINT
void filterbank_compute_bank(FilterBank *bank, float *ps, float *mel)
{
   int i;
   for (i=0;i<bank->nb_banks;i++)
      mel[i] = 0;

   for (i=0;i<bank->len;i++)
   {
      int id = bank->bank_left[i];
      mel[id] += bank->filter_left[i]*ps[i];
      id = bank->bank_right[i];
      mel[id] += bank->filter_right[i]*ps[i];
   }
   for (i=0;i<bank->nb_banks;i++)
      mel[i] *= bank->scaling[i];
}

void filterbank_compute_psd(FilterBank *bank, float *mel, float *ps)
{
   int i;
   for (i=0;i<bank->len;i++)
   {
      int id = bank->bank_left[i];
      ps[i] = mel[id]*bank->filter_left[i];
      id = bank->bank_right[i];
      ps[i] += mel[id]*bank->filter_right[i];
   }
}

void filterbank_psy_smooth(FilterBank *bank, float *ps, float *mask)
{
   /* Low freq slope: 14 dB/Bark*/
   /* High freq slope: 9 dB/Bark*/
   /* Noise vs tone: 5 dB difference */
   /* FIXME: Temporary kludge */
   float bark[100];
   int i;
   /* Assumes 1/3 Bark resolution */
   float decay_low = 0.34145f;
   float decay_high = 0.50119f;
   filterbank_compute_bank(bank, ps, bark);
   for (i=1;i<bank->nb_banks;i++)
   {
      /*float decay_high = 13-1.6*log10(bark[i-1]);
      decay_high = pow(10,(-decay_high/30.f));*/
      bark[i] = bark[i] + decay_high*bark[i-1];
   }
   for (i=bank->nb_banks-2;i>=0;i--)
   {
      bark[i] = bark[i] + decay_low*bark[i+1];
   }
   filterbank_compute_psd(bank, bark, mask);
}

#endif
