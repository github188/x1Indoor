#include "ace_config.h"

#include <math.h>
#include "ace_preprocess.h"
#include "ace_echo.h"
#include "ace_arch.h"
#include "fftwrap.h"
#include "filterbank.h"
#include "math_approx.h"
#include "os_support.h"

#ifndef M_PI
#define M_PI 3.14159263
#endif

#define LOUDNESS_EXP 5.f
#define AMP_SCALE .001f
#define AMP_SCALE_1 1000.f
      
#define NB_BANDS 24

#define SPEECH_PROB_START_DEFAULT       QCONST16(0.35f,15)
#define SPEECH_PROB_CONTINUE_DEFAULT    QCONST16(0.20f,15)

#ifndef NULL
#define NULL 0
#endif

#define SQR(x) ((x)*(x))
#define SQR16(x) (MULT16_16((x),(x)))
#define SQR16_Q15(x) (MULT16_16_Q15((x),(x)))

#ifdef FIXED_POINT
static inline ace_word16_t DIV32_16_Q8(ace_word32_t a, ace_word32_t b)
{
   if (SHR32(a,7) >= b)
   {
      return 32767;
   } else {
      if (b>=QCONST32(1,23))
      {
         a = SHR32(a,8);
         b = SHR32(b,8);
      }
      if (b>=QCONST32(1,19))
      {
         a = SHR32(a,4);
         b = SHR32(b,4);
      }
      if (b>=QCONST32(1,15))
      {
         a = SHR32(a,4);
         b = SHR32(b,4);
      }
      a = SHL32(a,8);
      return PDIV32_16(a,b);
   }
   
}
static inline ace_word16_t DIV32_16_Q15(ace_word32_t a, ace_word32_t b)
{
   if (SHR32(a,15) >= b)
   {
      return 32767;
   } else {
      if (b>=QCONST32(1,23))
      {
         a = SHR32(a,8);
         b = SHR32(b,8);
      }
      if (b>=QCONST32(1,19))
      {
         a = SHR32(a,4);
         b = SHR32(b,4);
      }
      if (b>=QCONST32(1,15))
      {
         a = SHR32(a,4);
         b = SHR32(b,4);
      }
      a = SHL32(a,15)-a;
      return DIV32_16(a,b);
   }
}
#define SNR_SCALING 256.f
#define SNR_SCALING_1 0.0039062f
#define SNR_SHIFT 8

#define FRAC_SCALING 32767.f
#define FRAC_SCALING_1 3.0518e-05
#define FRAC_SHIFT 1

#define EXPIN_SCALING 2048.f
#define EXPIN_SCALING_1 0.00048828f
#define EXPIN_SHIFT 11
#define EXPOUT_SCALING_1 1.5259e-05

#define NOISE_SHIFT 7

#else

#define DIV32_16_Q8(a,b) ((a)/(b))
#define DIV32_16_Q15(a,b) ((a)/(b))
#define SNR_SCALING 1.f
#define SNR_SCALING_1 1.f
#define SNR_SHIFT 0
#define FRAC_SCALING 1.f
#define FRAC_SCALING_1 1.f
#define FRAC_SHIFT 0
#define NOISE_SHIFT 0

#define EXPIN_SCALING 1.f
#define EXPIN_SCALING_1 1.f
#define EXPOUT_SCALING_1 1.f

#endif

/** ACE pre-processor state. */
struct ACEPreprocessState_ {
   /* Basic info */
   int    frame_size;        /**< Number of samples processed each time */
   int    ps_size;           /**< Number of points in the power spectrum */
   int    sampling_rate;     /**< Sampling rate of the input/output */
   int    nbands;
   FilterBank *bank;
   
   /* Parameters */
   int    denoise_enabled;
   int    vad_enabled;
   int    dereverb_enabled;
   ace_word16_t  reverb_decay;
   ace_word16_t  reverb_level;
   ace_word16_t speech_prob_start;
   ace_word16_t speech_prob_continue;
   int    noise_suppress;
   int    echo_suppress;
   int    echo_suppress_active;
   ACEEchoState *echo_state;
   
   /* DSP-related arrays */
   ace_word16_t *frame;      /**< Processing frame (2*ps_size) */
   ace_word16_t *ft;         /**< Processing frame in freq domain (2*ps_size) */
   ace_word32_t *ps;         /**< Current power spectrum */
   ace_word16_t *gain2;      /**< Adjusted gains */
   ace_word16_t *gain_floor; /**< Minimum gain allowed */
   ace_word16_t *window;     /**< Analysis/Synthesis window */
   ace_word32_t *noise;      /**< Noise estimate */
   ace_word32_t *reverb_estimate; /**< Estimate of reverb energy */
   ace_word32_t *old_ps;     /**< Power spectrum for last frame */
   ace_word16_t *gain;       /**< Ephraim Malah gain */
   ace_word16_t *prior;      /**< A-priori SNR */
   ace_word16_t *post;       /**< A-posteriori SNR */

   ace_word32_t *S;          /**< Smoothed power spectrum */
   ace_word32_t *Smin;       /**< See Cohen paper */
   ace_word32_t *Stmp;       /**< See Cohen paper */
   int *update_prob;         /**< Probability of speech presence for noise update */

   ace_word16_t *zeta;       /**< Smoothed a priori SNR */
   ace_word32_t *echo_noise;
   ace_word32_t *residual_echo;

   /* Misc */
   ace_word16_t *inbuf;      /**< Input buffer (overlapped analysis) */
   ace_word16_t *outbuf;     /**< Output buffer (for overlap and add) */

   /* AGC stuff, only for floating point for now */
#ifndef FIXED_POINT
   int    agc_enabled;
   float  agc_level;
   float  loudness_accum;
   float *loudness_weight;   /**< Perceptual loudness curve */
   float  loudness;          /**< Loudness estimate */
   float  agc_gain;          /**< Current AGC gain */
   int    nb_loudness_adapt; /**< Number of frames used for loudness adaptation so far */
   float  max_gain;          /**< Maximum gain allowed */
   float  max_increase_step; /**< Maximum increase in gain from one frame to another */
   float  max_decrease_step; /**< Maximum decrease in gain from one frame to another */
   float  prev_loudness;     /**< Loudness of previous frame */
   float  init_max;          /**< Current gain limit during initialisation */
#endif
   int    nb_adapt;          /**< Number of frames used for adaptation so far */
   int    was_speech;
   int    min_count;         /**< Number of frames processed so far */
   void  *fft_lookup;        /**< Lookup table for the FFT */
#ifdef FIXED_POINT
   int    frame_shift;
#endif
};


static void conj_window(ace_word16_t *w, int len)
{
   int i;
   for (i=0;i<len;i++)
   {
      ace_word16_t tmp;
#ifdef FIXED_POINT
      ace_word16_t x = DIV32_16(MULT16_16(32767,i),len);
#else      
      ace_word16_t x = DIV32_16(MULT16_16(QCONST16(4.f,13),i),len);
#endif
      int inv=0;
      if (x<QCONST16(1.f,13))
      {
      } else if (x<QCONST16(2.f,13))
      {
         x=QCONST16(2.f,13)-x;
         inv=1;
      } else if (x<QCONST16(3.f,13))
      {
         x=x-QCONST16(2.f,13);
         inv=1;
      } else {
         x=QCONST16(2.f,13)-x+QCONST16(2.f,13); /* 4 - x */
      }
      x = MULT16_16_Q14(QCONST16(1.271903f,14), x);
      tmp = SQR16_Q15(QCONST16(.5f,15)-MULT16_16_P15(QCONST16(.5f,15),ace_cos_norm(SHL32(EXTEND32(x),2))));
      if (inv)
         tmp=SUB16(Q15_ONE,tmp);
      w[i]=ace_sqrt(SHL32(EXTEND32(tmp),15));
   }
}

      
#ifdef FIXED_POINT
/* This function approximates the gain function 
   y = gamma(1.25)^2 * M(-.25;1;-x) / sqrt(x)  
   which multiplied by xi/(1+xi) is the optimal gain
   in the loudness domain ( sqrt[amplitude] )
   Input in Q11 format, output in Q15
*/
static inline ace_word32_t hypergeom_gain(ace_word32_t xx)
{
   int ind;
   ace_word16_t frac;
   /* Q13 table */
   static const ace_word16_t table[21] = {
       6730,  8357,  9868, 11267, 12563, 13770, 14898,
      15959, 16961, 17911, 18816, 19682, 20512, 21311,
      22082, 22827, 23549, 24250, 24931, 25594, 26241};
      ind = SHR32(xx,10);
      if (ind<0)
         return Q15_ONE;
      if (ind>19)
         return ADD32(EXTEND32(Q15_ONE),EXTEND32(DIV32_16(QCONST32(.1296,23), SHR32(xx,EXPIN_SHIFT-SNR_SHIFT))));
      frac = SHL32(xx-SHL32(ind,10),5);
      return SHL32(DIV32_16(PSHR32(MULT16_16(Q15_ONE-frac,table[ind]) + MULT16_16(frac,table[ind+1]),7),(ace_sqrt(SHL32(xx,15)+6711))),7);
}

static inline ace_word16_t qcurve(ace_word16_t x)
{
   x = MAX16(x, 1);
   return DIV32_16(SHL32(EXTEND32(32767),9),ADD16(512,MULT16_16_Q15(QCONST16(.60f,15),DIV32_16(32767,x))));
}

/* Compute the gain floor based on different floors for the background noise and residual echo */
static void compute_gain_floor(int noise_suppress, int effective_echo_suppress, ace_word32_t *noise, ace_word32_t *echo, ace_word16_t *gain_floor, int len)
{
   int i;
   
   if (noise_suppress > effective_echo_suppress)
   {
      ace_word16_t noise_gain, gain_ratio;
      noise_gain = EXTRACT16(MIN32(Q15_ONE,SHR32(ace_exp(MULT16_16(QCONST16(0.11513,11),noise_suppress)),1)));
      gain_ratio = EXTRACT16(MIN32(Q15_ONE,SHR32(ace_exp(MULT16_16(QCONST16(.2302585f,11),effective_echo_suppress-noise_suppress)),1)));

      /* gain_floor = sqrt [ (noise*noise_floor + echo*echo_floor) / (noise+echo) ] */
      for (i=0;i<len;i++)
         gain_floor[i] = MULT16_16_Q15(noise_gain,
                                       ace_sqrt(SHL32(EXTEND32(DIV32_16_Q15(PSHR32(noise[i],NOISE_SHIFT) + MULT16_32_Q15(gain_ratio,echo[i]),
                                             (1+PSHR32(noise[i],NOISE_SHIFT) + echo[i]) )),15)));
   } else {
      ace_word16_t echo_gain, gain_ratio;
      echo_gain = EXTRACT16(MIN32(Q15_ONE,SHR32(ace_exp(MULT16_16(QCONST16(0.11513,11),effective_echo_suppress)),1)));
      gain_ratio = EXTRACT16(MIN32(Q15_ONE,SHR32(ace_exp(MULT16_16(QCONST16(.2302585f,11),noise_suppress-effective_echo_suppress)),1)));

      /* gain_floor = sqrt [ (noise*noise_floor + echo*echo_floor) / (noise+echo) ] */
      for (i=0;i<len;i++)
         gain_floor[i] = MULT16_16_Q15(echo_gain,
                                       ace_sqrt(SHL32(EXTEND32(DIV32_16_Q15(MULT16_32_Q15(gain_ratio,PSHR32(noise[i],NOISE_SHIFT)) + echo[i],
                                             (1+PSHR32(noise[i],NOISE_SHIFT) + echo[i]) )),15)));
   }
}

#else
/* This function approximates the gain function 
   y = gamma(1.25)^2 * M(-.25;1;-x) / sqrt(x)  
   which multiplied by xi/(1+xi) is the optimal gain
   in the loudness domain ( sqrt[amplitude] )
*/
static inline ace_word32_t hypergeom_gain(ace_word32_t xx)
{
   int ind;
   float integer, frac;
   float x;
   static const float table[21] = {
      0.82157f, 1.02017f, 1.20461f, 1.37534f, 1.53363f, 1.68092f, 1.81865f,
      1.94811f, 2.07038f, 2.18638f, 2.29688f, 2.40255f, 2.50391f, 2.60144f,
      2.69551f, 2.78647f, 2.87458f, 2.96015f, 3.04333f, 3.12431f, 3.20326f};
      x = EXPIN_SCALING_1*xx;
      integer = floor(2*x);
      ind = (int)integer;
      if (ind<0)
         return FRAC_SCALING;
      if (ind>19)
         return FRAC_SCALING*(1+.1296/x);
      frac = 2*x-integer;
      return FRAC_SCALING*((1-frac)*table[ind] + frac*table[ind+1])/sqrt(x+.0001f);
}

static inline ace_word16_t qcurve(ace_word16_t x)
{
   return 1.f/(1.f+.15f/(SNR_SCALING_1*x));
}

static void compute_gain_floor(int noise_suppress, int effective_echo_suppress, ace_word32_t *noise, ace_word32_t *echo, ace_word16_t *gain_floor, int len)
{
   int i;
   float echo_floor;
   float noise_floor;

   noise_floor = exp(.2302585f*noise_suppress);
   echo_floor = exp(.2302585f*effective_echo_suppress);

   /* Compute the gain floor based on different floors for the background noise and residual echo */
   for (i=0;i<len;i++)
      gain_floor[i] = FRAC_SCALING*sqrt(noise_floor*PSHR32(noise[i],NOISE_SHIFT) + echo_floor*echo[i])/sqrt(1+PSHR32(noise[i],NOISE_SHIFT) + echo[i]);
}

#endif
ACEPreprocessState *ace_preprocess_state_init(int frame_size, int noiselevel, int echolevel)
{
   int i;
   int N, N3, N4, M;

   ACEPreprocessState *st = (ACEPreprocessState *)ace_alloc(sizeof(ACEPreprocessState));
   st->frame_size = frame_size;

   /* Round ps_size down to the nearest power of two */
#if 0
   i=1;
   st->ps_size = st->frame_size;
   while(1)
   {
      if (st->ps_size & ~i)
      {
         st->ps_size &= ~i;
         i<<=1;
      } else {
         break;
      }
   }
   
   
   if (st->ps_size < 3*st->frame_size/4)
      st->ps_size = st->ps_size * 3 / 2;
#else
   st->ps_size = st->frame_size;
#endif

   N = st->ps_size;
   N3 = 2*N - st->frame_size;
   N4 = st->frame_size - N3;
   
   st->sampling_rate = 8000;
   st->denoise_enabled = 1;
   st->vad_enabled = 0;
   st->dereverb_enabled = 1;
   st->reverb_decay = 0;
   st->reverb_level = 0;
   st->noise_suppress = noiselevel;
   st->echo_suppress = echolevel;
   st->echo_suppress_active = echolevel+10;

   st->speech_prob_start = SPEECH_PROB_START_DEFAULT;
   st->speech_prob_continue = SPEECH_PROB_CONTINUE_DEFAULT;

   st->echo_state = NULL;
   
   st->nbands = NB_BANDS;
   M = st->nbands;
   st->bank = filterbank_new(M, 8000, N, 1);
   
   st->frame = (ace_word16_t*)ace_alloc(2*N*sizeof(ace_word16_t));
   st->window = (ace_word16_t*)ace_alloc(2*N*sizeof(ace_word16_t));
   st->ft = (ace_word16_t*)ace_alloc(2*N*sizeof(ace_word16_t));
   
   st->ps = (ace_word32_t*)ace_alloc((N+M)*sizeof(ace_word32_t));
   st->noise = (ace_word32_t*)ace_alloc((N+M)*sizeof(ace_word32_t));
   st->echo_noise = (ace_word32_t*)ace_alloc((N+M)*sizeof(ace_word32_t));
   st->residual_echo = (ace_word32_t*)ace_alloc((N+M)*sizeof(ace_word32_t));
   st->reverb_estimate = (ace_word32_t*)ace_alloc((N+M)*sizeof(ace_word32_t));
   st->old_ps = (ace_word32_t*)ace_alloc((N+M)*sizeof(ace_word32_t));
   st->prior = (ace_word16_t*)ace_alloc((N+M)*sizeof(ace_word16_t));
   st->post = (ace_word16_t*)ace_alloc((N+M)*sizeof(ace_word16_t));
   st->gain = (ace_word16_t*)ace_alloc((N+M)*sizeof(ace_word16_t));
   st->gain2 = (ace_word16_t*)ace_alloc((N+M)*sizeof(ace_word16_t));
   st->gain_floor = (ace_word16_t*)ace_alloc((N+M)*sizeof(ace_word16_t));
   st->zeta = (ace_word16_t*)ace_alloc((N+M)*sizeof(ace_word16_t));
   
   st->S = (ace_word32_t*)ace_alloc(N*sizeof(ace_word32_t));
   st->Smin = (ace_word32_t*)ace_alloc(N*sizeof(ace_word32_t));
   st->Stmp = (ace_word32_t*)ace_alloc(N*sizeof(ace_word32_t));
   st->update_prob = (int*)ace_alloc(N*sizeof(int));
   
   st->inbuf = (ace_word16_t*)ace_alloc(N3*sizeof(ace_word16_t));
   st->outbuf = (ace_word16_t*)ace_alloc(N3*sizeof(ace_word16_t));

   conj_window(st->window, 2*N3);
   for (i=2*N3;i<2*st->ps_size;i++)
      st->window[i]=Q15_ONE;
   
   if (N4>0)
   {
      for (i=N3-1;i>=0;i--)
      {
         st->window[i+N3+N4]=st->window[i+N3];
         st->window[i+N3]=1;
      }
   }
   for (i=0;i<N+M;i++)
   {
      st->noise[i]=QCONST32(1.f,NOISE_SHIFT);
      st->reverb_estimate[i]=0;
      st->old_ps[i]=1;
      st->gain[i]=Q15_ONE;
      st->post[i]=SHL16(1, SNR_SHIFT);
      st->prior[i]=SHL16(1, SNR_SHIFT);
   }

   for (i=0;i<N;i++)
      st->update_prob[i] = 1;
   for (i=0;i<N3;i++)
   {
      st->inbuf[i]=0;
      st->outbuf[i]=0;
   }
#ifndef FIXED_POINT
   st->agc_enabled = 0;
   st->agc_level = 8000;
   st->loudness_weight = (float*)ace_alloc(N*sizeof(float));
   for (i=0;i<N;i++)
   {
      float ff=((float)i)*.5*8000/((float)N);
      /*st->loudness_weight[i] = .5f*(1.f/(1.f+ff/8000.f))+1.f*exp(-.5f*(ff-3800.f)*(ff-3800.f)/9e5f);*/
      st->loudness_weight[i] = .35f-.35f*ff/16000.f+.73f*exp(-.5f*(ff-3800)*(ff-3800)/9e5f);
      if (st->loudness_weight[i]<.01f)
         st->loudness_weight[i]=.01f;
      st->loudness_weight[i] *= st->loudness_weight[i];
   }
   /*st->loudness = pow(AMP_SCALE*st->agc_level,LOUDNESS_EXP);*/
   st->loudness = 1e-15;
   st->agc_gain = 1;
   st->nb_loudness_adapt = 0;
   st->max_gain = 30;
   st->max_increase_step = exp(0.11513f * 12.*st->frame_size / st->sampling_rate);
   st->max_decrease_step = exp(-0.11513f * 40.*st->frame_size / st->sampling_rate);
   st->prev_loudness = 1;
   st->init_max = 1;
#endif
   st->was_speech = 0;

   st->fft_lookup = ace_fft_init(2*N);

   st->nb_adapt=0;
   st->min_count=0;
   return st;
}

void ace_preprocess_state_destroy(ACEPreprocessState *st)
{
   ace_free(st->frame);
   ace_free(st->ft);
   ace_free(st->ps);
   ace_free(st->gain2);
   ace_free(st->gain_floor);
   ace_free(st->window);
   ace_free(st->noise);
   ace_free(st->reverb_estimate);
   ace_free(st->old_ps);
   ace_free(st->gain);
   ace_free(st->prior);
   ace_free(st->post);
#ifndef FIXED_POINT
   ace_free(st->loudness_weight);
#endif
   ace_free(st->echo_noise);
   ace_free(st->residual_echo);

   ace_free(st->S);
   ace_free(st->Smin);
   ace_free(st->Stmp);
   ace_free(st->update_prob);
   ace_free(st->zeta);

   ace_free(st->inbuf);
   ace_free(st->outbuf);

   ace_fft_destroy(st->fft_lookup);
   filterbank_destroy(st->bank);
   ace_free(st);
}

/* FIXME: The AGC doesn't work yet with fixed-point*/
#ifndef FIXED_POINT
static void ace_compute_agc(ACEPreprocessState *st, ace_word16_t Pframe, ace_word16_t *ft)
{
   int i;
   int N = st->ps_size;
   float target_gain;
   float loudness=1.f;
   float rate;
   
   for (i=2;i<N;i++)
   {
      loudness += 2.f*N*st->ps[i]* st->loudness_weight[i];
   }
   loudness=sqrt(loudness);
      /*if (loudness < 2*pow(st->loudness, 1.0/LOUDNESS_EXP) &&
   loudness*2 > pow(st->loudness, 1.0/LOUDNESS_EXP))*/
   if (Pframe>.3f)
   {
      st->nb_loudness_adapt++;
      /*rate=2.0f*Pframe*Pframe/(1+st->nb_loudness_adapt);*/
      rate = .03*Pframe*Pframe;
      st->loudness = (1-rate)*st->loudness + (rate)*pow(AMP_SCALE*loudness, LOUDNESS_EXP);
      st->loudness_accum = (1-rate)*st->loudness_accum + rate;
      if (st->init_max < st->max_gain && st->nb_adapt > 20)
         st->init_max *= 1.f + .1f*Pframe*Pframe;
   }
   /*printf ("%f %f %f %f\n", Pframe, loudness, pow(st->loudness, 1.0f/LOUDNESS_EXP), st->loudness2);*/
   
   target_gain = AMP_SCALE*st->agc_level*pow(st->loudness/(1e-4+st->loudness_accum), -1.0f/LOUDNESS_EXP);

   if ((Pframe>.5  && st->nb_adapt > 20) || target_gain < st->agc_gain)
   {
      if (target_gain > st->max_increase_step*st->agc_gain)
         target_gain = st->max_increase_step*st->agc_gain;
      if (target_gain < st->max_decrease_step*st->agc_gain && loudness < 10*st->prev_loudness)
         target_gain = st->max_decrease_step*st->agc_gain;
      if (target_gain > st->max_gain)
         target_gain = st->max_gain;
      if (target_gain > st->init_max)
         target_gain = st->init_max;
   
      st->agc_gain = target_gain;
   }
   /*fprintf (stderr, "%f %f %f\n", loudness, (float)AMP_SCALE_1*pow(st->loudness, 1.0f/LOUDNESS_EXP), st->agc_gain);*/
      
   for (i=0;i<2*N;i++)
      ft[i] *= st->agc_gain;
   st->prev_loudness = loudness;
}
#endif

static void preprocess_analysis(ACEPreprocessState *st, ace_int16_t *x)
{
   int i;
   int N = st->ps_size;
   int N3 = 2*N - st->frame_size;
   int N4 = st->frame_size - N3;
   ace_word32_t *ps=st->ps;

   /* 'Build' input frame */
   for (i=0;i<N3;i++)
      st->frame[i]=st->inbuf[i];
   for (i=0;i<st->frame_size;i++)
      st->frame[N3+i]=x[i];
   
   /* Update inbuf */
   for (i=0;i<N3;i++)
      st->inbuf[i]=x[N4+i];

   /* Windowing */
   for (i=0;i<2*N;i++)
      st->frame[i] = MULT16_16_Q15(st->frame[i], st->window[i]);

#ifdef FIXED_POINT
   {
      ace_word16_t max_val=0;
      for (i=0;i<2*N;i++)
         max_val = MAX16(max_val, ABS16(st->frame[i]));
      st->frame_shift = 14-ace_ilog2(EXTEND32(max_val));
      for (i=0;i<2*N;i++)
         st->frame[i] = SHL16(st->frame[i], st->frame_shift);
   }
#endif
   
   /* Perform FFT */
   ace_fft(st->fft_lookup, st->frame, st->ft);
         
   /* Power spectrum */
   ps[0]=MULT16_16(st->ft[0],st->ft[0]);
   for (i=1;i<N;i++)
      ps[i]=MULT16_16(st->ft[2*i-1],st->ft[2*i-1]) + MULT16_16(st->ft[2*i],st->ft[2*i]);
   for (i=0;i<N;i++)
      st->ps[i] = PSHR32(st->ps[i], 2*st->frame_shift);

   filterbank_compute_bank32(st->bank, ps, ps+N);
}

static void update_noise_prob(ACEPreprocessState *st)
{
   int i;
   int min_range;
   int N = st->ps_size;

   for (i=1;i<N-1;i++)
      st->S[i] =  MULT16_32_Q15(QCONST16(.8f,15),st->S[i]) + MULT16_32_Q15(QCONST16(.05f,15),st->ps[i-1]) 
                      + MULT16_32_Q15(QCONST16(.1f,15),st->ps[i]) + MULT16_32_Q15(QCONST16(.05f,15),st->ps[i+1]);
   st->S[0] =  MULT16_32_Q15(QCONST16(.8f,15),st->S[0]) + MULT16_32_Q15(QCONST16(.2f,15),st->ps[0]);
   st->S[N-1] =  MULT16_32_Q15(QCONST16(.8f,15),st->S[N-1]) + MULT16_32_Q15(QCONST16(.2f,15),st->ps[N-1]);
   
   if (st->nb_adapt==1)
   {
      for (i=0;i<N;i++)
         st->Smin[i] = st->Stmp[i] = 0;
   }

   if (st->nb_adapt < 100)
      min_range = 15;
   else if (st->nb_adapt < 1000)
      min_range = 50;
   else if (st->nb_adapt < 10000)
      min_range = 150;
   else
      min_range = 300;
   if (st->min_count > min_range)
   {
      st->min_count = 0;
      for (i=0;i<N;i++)
      {
         st->Smin[i] = MIN32(st->Stmp[i], st->S[i]);
         st->Stmp[i] = st->S[i];
      }
   } else {
      for (i=0;i<N;i++)
      {
         st->Smin[i] = MIN32(st->Smin[i], st->S[i]);
         st->Stmp[i] = MIN32(st->Stmp[i], st->S[i]);      
      }
   }
   for (i=0;i<N;i++)
   {
      if (MULT16_32_Q15(QCONST16(.4f,15),st->S[i]) > ADD32(st->Smin[i],EXTEND32(20)))
         st->update_prob[i] = 1;
      else
         st->update_prob[i] = 0;
      /*fprintf (stderr, "%f ", st->S[i]/st->Smin[i]);*/
      /*fprintf (stderr, "%f ", st->update_prob[i]);*/
   }

}

#define NOISE_OVERCOMPENS 1.

void ace_echo_get_residual(ACEEchoState *st, ace_word32_t *Yout, int len);

int ace_preprocess(ACEPreprocessState *st, ace_int16_t *x, ace_int32_t *echo)
{
   return ace_preprocess_run(st, x);
}

int ace_preprocess_run(ACEPreprocessState *st, ace_int16_t *x)
{
   int i;
   int M;
   int N = st->ps_size;
   int N3 = 2*N - st->frame_size;
   int N4 = st->frame_size - N3;
   ace_word32_t *ps=st->ps;
   ace_word32_t Zframe;
   ace_word16_t Pframe;
   ace_word16_t beta, beta_1;
   ace_word16_t effective_echo_suppress;
   
   st->nb_adapt++;
   if (st->nb_adapt>20000)
      st->nb_adapt = 20000;
   st->min_count++;
   
   beta = MAX16(QCONST16(.03,15),DIV32_16(Q15_ONE,st->nb_adapt));
   beta_1 = Q15_ONE-beta;
   M = st->nbands;
   /* Deal with residual echo if provided */
   if (st->echo_state)
   {
      ace_echo_get_residual(st->echo_state, st->residual_echo, N);
#ifndef FIXED_POINT
      /* If there are NaNs or ridiculous values, it'll show up in the DC and we just reset everything to zero */
      if (!(st->residual_echo[0] >=0 && st->residual_echo[0]<N*1e9f))
      {
         for (i=0;i<N;i++)
            st->residual_echo[i] = 0;
      }
#endif
      for (i=0;i<N;i++)
         st->echo_noise[i] = MAX32(MULT16_32_Q15(QCONST16(.6f,15),st->echo_noise[i]), st->residual_echo[i]);
      filterbank_compute_bank32(st->bank, st->echo_noise, st->echo_noise+N);
   } else {
      for (i=0;i<N+M;i++)
         st->echo_noise[i] = 0;
   }
   preprocess_analysis(st, x);

   update_noise_prob(st);

   /* Noise estimation always updated for the 10 first frames */
   /*if (st->nb_adapt<10)
   {
      for (i=1;i<N-1;i++)
         st->update_prob[i] = 0;
   }
   */
   
   /* Update the noise estimate for the frequencies where it can be */
   for (i=0;i<N;i++)
   {
      if (!st->update_prob[i] || st->ps[i] < PSHR32(st->noise[i], NOISE_SHIFT))
         st->noise[i] = MAX32(EXTEND32(0),MULT16_32_Q15(beta_1,st->noise[i]) + MULT16_32_Q15(beta,SHL32(st->ps[i],NOISE_SHIFT)));
   }
   filterbank_compute_bank32(st->bank, st->noise, st->noise+N);

   /* Special case for first frame */
   if (st->nb_adapt==1)
      for (i=0;i<N+M;i++)
         st->old_ps[i] = ps[i];

   /* Compute a posteriori SNR */
   for (i=0;i<N+M;i++)
   {
      ace_word16_t gamma;
      
      /* Total noise estimate including residual echo and reverberation */
      ace_word32_t tot_noise = ADD32(ADD32(ADD32(EXTEND32(1), PSHR32(st->noise[i],NOISE_SHIFT)) , st->echo_noise[i]) , st->reverb_estimate[i]);
      
      /* A posteriori SNR = ps/noise - 1*/
      st->post[i] = SUB16(DIV32_16_Q8(ps[i],tot_noise), QCONST16(1.f,SNR_SHIFT));
      st->post[i]=MIN16(st->post[i], QCONST16(100.f,SNR_SHIFT));
      
      /* Computing update gamma = .1 + .9*(old/(old+noise))^2 */
      gamma = QCONST16(.1f,15)+MULT16_16_Q15(QCONST16(.89f,15),SQR16_Q15(DIV32_16_Q15(st->old_ps[i],ADD32(st->old_ps[i],tot_noise))));
      
      /* A priori SNR update = gamma*max(0,post) + (1-gamma)*old/noise */
      st->prior[i] = EXTRACT16(PSHR32(ADD32(MULT16_16(gamma,MAX16(0,st->post[i])), MULT16_16(Q15_ONE-gamma,DIV32_16_Q8(st->old_ps[i],tot_noise))), 15));
      st->prior[i]=MIN16(st->prior[i], QCONST16(100.f,SNR_SHIFT));
   }

   /*print_vec(st->post, N+M, "");*/

   /* Recursive average of the a priori SNR. A bit smoothed for the psd components */
   st->zeta[0] = PSHR32(ADD32(MULT16_16(QCONST16(.7f,15),st->zeta[0]), MULT16_16(QCONST16(.3f,15),st->prior[0])),15);
   for (i=1;i<N-1;i++)
      st->zeta[i] = PSHR32(ADD32(ADD32(ADD32(MULT16_16(QCONST16(.7f,15),st->zeta[i]), MULT16_16(QCONST16(.15f,15),st->prior[i])),
                           MULT16_16(QCONST16(.075f,15),st->prior[i-1])), MULT16_16(QCONST16(.075f,15),st->prior[i+1])),15);
   for (i=N-1;i<N+M;i++)
      st->zeta[i] = PSHR32(ADD32(MULT16_16(QCONST16(.7f,15),st->zeta[i]), MULT16_16(QCONST16(.3f,15),st->prior[i])),15);

   /* Speech probability of presence for the entire frame is based on the average filterbank a priori SNR */
   Zframe = 0;
   for (i=N;i<N+M;i++)
      Zframe = ADD32(Zframe, EXTEND32(st->zeta[i]));
   Pframe = QCONST16(.1f,15)+MULT16_16_Q15(QCONST16(.899f,15),qcurve(DIV32_16(Zframe,st->nbands)));
   
   effective_echo_suppress = EXTRACT16(PSHR32(ADD32(MULT16_16(SUB16(Q15_ONE,Pframe), st->echo_suppress), MULT16_16(Pframe, st->echo_suppress_active)),15));
   
   compute_gain_floor(st->noise_suppress, effective_echo_suppress, st->noise+N, st->echo_noise+N, st->gain_floor+N, M);
         
   /* Compute Ephraim & Malah gain speech probability of presence for each critical band (Bark scale) 
      Technically this is actually wrong because the EM gaim assumes a slightly different probability 
      distribution */
   for (i=N;i<N+M;i++)
   {
      /* See EM and Cohen papers*/
      ace_word32_t theta;
      /* Gain from hypergeometric function */
      ace_word32_t MM;
      /* Weiner filter gain */
      ace_word16_t prior_ratio;
      /* a priority probability of speech presence based on Bark sub-band alone */
      ace_word16_t P1;
      /* Speech absence a priori probability (considering sub-band and frame) */
      ace_word16_t q;
#ifdef FIXED_POINT
      ace_word16_t tmp;
#endif
      
      prior_ratio = PDIV32_16(SHL32(EXTEND32(st->prior[i]), 15), ADD16(st->prior[i], SHL32(1,SNR_SHIFT)));
      theta = MULT16_32_P15(prior_ratio, QCONST32(1.f,EXPIN_SHIFT)+SHL32(EXTEND32(st->post[i]),EXPIN_SHIFT-SNR_SHIFT));

      MM = hypergeom_gain(theta);
      /* Gain with bound */
      st->gain[i] = EXTRACT16(MIN32(Q15_ONE, MULT16_32_Q15(prior_ratio, MM)));
      /* Save old Bark power spectrum */
      st->old_ps[i] = MULT16_32_P15(QCONST16(.2f,15),st->old_ps[i]) + MULT16_32_P15(MULT16_16_P15(QCONST16(.8f,15),SQR16_Q15(st->gain[i])),ps[i]);

      P1 = QCONST16(.199f,15)+MULT16_16_Q15(QCONST16(.8f,15),qcurve (st->zeta[i]));
      q = Q15_ONE-MULT16_16_Q15(Pframe,P1);
#ifdef FIXED_POINT
      theta = MIN32(theta, EXTEND32(32767));
/*Q8*/tmp = MULT16_16_Q15((SHL32(1,SNR_SHIFT)+st->prior[i]),EXTRACT16(MIN32(Q15ONE,SHR32(ace_exp(-EXTRACT16(theta)),1))));
      tmp = MIN16(QCONST16(3.,SNR_SHIFT), tmp); /* Prevent overflows in the next line*/
/*Q8*/tmp = EXTRACT16(PSHR32(MULT16_16(PDIV32_16(SHL32(EXTEND32(q),8),(Q15_ONE-q)),tmp),8));
      st->gain2[i]=DIV32_16(SHL32(EXTEND32(32767),SNR_SHIFT), ADD16(256,tmp));
#else
      st->gain2[i]=1/(1.f + (q/(1.f-q))*(1+st->prior[i])*exp(-theta));
#endif
   }
   /* Convert the EM gains and speech prob to linear frequency */
   filterbank_compute_psd16(st->bank,st->gain2+N, st->gain2);
   filterbank_compute_psd16(st->bank,st->gain+N, st->gain);
   
   /* Use 1 for linear gain resolution (best) or 0 for Bark gain resolution (faster) */
   if (1)
   {
      filterbank_compute_psd16(st->bank,st->gain_floor+N, st->gain_floor);
   
      /* Compute gain according to the Ephraim-Malah algorithm -- linear frequency */
      for (i=0;i<N;i++)
      {
         ace_word32_t MM;
         ace_word32_t theta;
         ace_word16_t prior_ratio;
         ace_word16_t tmp;
         ace_word16_t p;
         ace_word16_t g;
         
         /* Wiener filter gain */
         prior_ratio = PDIV32_16(SHL32(EXTEND32(st->prior[i]), 15), ADD16(st->prior[i], SHL32(1,SNR_SHIFT)));
         theta = MULT16_32_P15(prior_ratio, QCONST32(1.f,EXPIN_SHIFT)+SHL32(EXTEND32(st->post[i]),EXPIN_SHIFT-SNR_SHIFT));

         /* Optimal estimator for loudness domain */
         MM = hypergeom_gain(theta);
         /* EM gain with bound */
         g = EXTRACT16(MIN32(Q15_ONE, MULT16_32_Q15(prior_ratio, MM)));
         /* Interpolated speech probability of presence */
         p = st->gain2[i];
                  
         /* Constrain the gain to be close to the Bark scale gain */
         if (MULT16_16_Q15(QCONST16(.333f,15),g) > st->gain[i])
            g = MULT16_16(3,st->gain[i]);
         st->gain[i] = g;
         
         /* Save old power spectrum */
         st->old_ps[i] = MULT16_32_P15(QCONST16(.2f,15),st->old_ps[i]) + MULT16_32_P15(MULT16_16_P15(QCONST16(.8f,15),SQR16_Q15(st->gain[i])),ps[i]);
         
         /* Apply gain floor */
         if (st->gain[i] < st->gain_floor[i])
            st->gain[i] = st->gain_floor[i];

         /* Exponential decay model for reverberation (unused) */
         /*st->reverb_estimate[i] = st->reverb_decay*st->reverb_estimate[i] + st->reverb_decay*st->reverb_level*st->gain[i]*st->gain[i]*st->ps[i];*/
         
         /* Take into account speech probability of presence (loudness domain MMSE estimator) */
         /* gain2 = [p*sqrt(gain)+(1-p)*sqrt(gain _floor) ]^2 */
         tmp = MULT16_16_P15(p,ace_sqrt(SHL32(EXTEND32(st->gain[i]),15))) + MULT16_16_P15(SUB16(Q15_ONE,p),ace_sqrt(SHL32(EXTEND32(st->gain_floor[i]),15)));
         st->gain2[i]=SQR16_Q15(tmp);

         /* Use this if you want a log-domain MMSE estimator instead */
         /*st->gain2[i] = pow(st->gain[i], p) * pow(st->gain_floor[i],1.f-p);*/
      }
   } else {
      for (i=N;i<N+M;i++)
      {
         ace_word16_t tmp;
         ace_word16_t p = st->gain2[i];
         st->gain[i] = MAX16(st->gain[i], st->gain_floor[i]);         
         tmp = MULT16_16_P15(p,ace_sqrt(SHL32(EXTEND32(st->gain[i]),15))) + MULT16_16_P15(SUB16(Q15_ONE,p),ace_sqrt(SHL32(EXTEND32(st->gain_floor[i]),15)));
         st->gain2[i]=SQR16_Q15(tmp);
      }
      filterbank_compute_psd16(st->bank,st->gain2+N, st->gain2);
   }
   
   /* If noise suppression is off, don't apply the gain (but then why call this in the first place!) */
   if (!st->denoise_enabled)
   {
      for (i=0;i<N+M;i++)
         st->gain2[i]=Q15_ONE;
   }
      
   /* Apply computed gain */
   for (i=1;i<N;i++)
   {
      st->ft[2*i-1] = MULT16_16_P15(st->gain2[i],st->ft[2*i-1]);
      st->ft[2*i] = MULT16_16_P15(st->gain2[i],st->ft[2*i]);
   }
   st->ft[0] = MULT16_16_P15(st->gain2[0],st->ft[0]);
   st->ft[2*N-1] = MULT16_16_P15(st->gain2[N-1],st->ft[2*N-1]);
   
   /*FIXME: This *will* not work for fixed-point */
#ifndef FIXED_POINT
   if (st->agc_enabled)
      ace_compute_agc(st, Pframe, st->ft);
#endif

   /* Inverse FFT with 1/N scaling */
   ace_ifft(st->fft_lookup, st->ft, st->frame);
   /* Scale back to original (lower) amplitude */
   for (i=0;i<2*N;i++)
      st->frame[i] = PSHR16(st->frame[i], st->frame_shift);

   /*FIXME: This *will* not work for fixed-point */
#ifndef FIXED_POINT
   if (st->agc_enabled)
   {
      float max_sample=0;
      for (i=0;i<2*N;i++)
         if (fabs(st->frame[i])>max_sample)
            max_sample = fabs(st->frame[i]);
      if (max_sample>28000.f)
      {
         float damp = 28000.f/max_sample;
         for (i=0;i<2*N;i++)
            st->frame[i] *= damp;
      }
   }
#endif
   
   /* Synthesis window (for WOLA) */
   for (i=0;i<2*N;i++)
      st->frame[i] = MULT16_16_Q15(st->frame[i], st->window[i]);

   /* Perform overlap and add */
   for (i=0;i<N3;i++)
      x[i] = st->outbuf[i] + st->frame[i];
   for (i=0;i<N4;i++)
      x[N3+i] = st->frame[N3+i];
   
   /* Update outbuf */
   for (i=0;i<N3;i++)
      st->outbuf[i] = st->frame[st->frame_size+i];

   /* FIXME: This VAD is a kludge */
   if (st->vad_enabled)
   {
      if (Pframe > st->speech_prob_start || (st->was_speech && Pframe > st->speech_prob_continue))
      {
         st->was_speech=1;
         return 1;
      } else
      {
         st->was_speech=0;
         return 0;
      }
   } else {
      return 1;
   }
}

void ace_preprocess_estimate_update(ACEPreprocessState *st, ace_int16_t *x)
{
   int i;
   int N = st->ps_size;
   int N3 = 2*N - st->frame_size;
   int M;
   ace_word32_t *ps=st->ps;

   M = st->nbands;
   st->min_count++;
   
   preprocess_analysis(st, x);

   update_noise_prob(st);
   
   for (i=1;i<N-1;i++)
   {
      if (!st->update_prob[i] || st->ps[i] < PSHR32(st->noise[i],NOISE_SHIFT))
      {
         st->noise[i] = MULT16_32_Q15(QCONST16(.95f,15),st->noise[i]) + MULT16_32_Q15(QCONST16(.05f,15),SHL32(st->ps[i],NOISE_SHIFT));
      }
   }

   for (i=0;i<N3;i++)
      st->outbuf[i] = MULT16_16_Q15(x[st->frame_size-N3+i],st->window[st->frame_size+i]);

   /* Save old power spectrum */
   for (i=0;i<N+M;i++)
      st->old_ps[i] = ps[i];

   for (i=0;i<N;i++)
      st->reverb_estimate[i] = MULT16_32_Q15(st->reverb_decay, st->reverb_estimate[i]);
}


int ace_preprocess_ctl(ACEPreprocessState *state, int request, void *ptr)
{
   int i;
   ACEPreprocessState *st;
   st=(ACEPreprocessState*)state;
   switch(request)
   {
   case ACE_PREPROCESS_SET_DENOISE:
      st->denoise_enabled = (*(ace_int32_t*)ptr);
      break;
   case ACE_PREPROCESS_GET_DENOISE:
      (*(ace_int32_t*)ptr) = st->denoise_enabled;
      break;
#ifndef FIXED_POINT
   case ACE_PREPROCESS_SET_AGC:
      st->agc_enabled = (*(ace_int32_t*)ptr);
      break;
   case ACE_PREPROCESS_GET_AGC:
      (*(ace_int32_t*)ptr) = st->agc_enabled;
      break;
#ifndef DISABLE_FLOAT_API
   case ACE_PREPROCESS_SET_AGC_LEVEL:
      st->agc_level = (*(float*)ptr);
      if (st->agc_level<1)
         st->agc_level=1;
      if (st->agc_level>32768)
         st->agc_level=32768;
      break;
   case ACE_PREPROCESS_GET_AGC_LEVEL:
      (*(float*)ptr) = st->agc_level;
      break;
#endif /* #ifndef DISABLE_FLOAT_API */
   case ACE_PREPROCESS_SET_AGC_INCREMENT:
      st->max_increase_step = exp(0.11513f * (*(ace_int32_t*)ptr)*st->frame_size / st->sampling_rate);
      break;
   case ACE_PREPROCESS_GET_AGC_INCREMENT:
      (*(ace_int32_t*)ptr) = floor(.5+8.6858*log(st->max_increase_step)*st->sampling_rate/st->frame_size);
      break;
   case ACE_PREPROCESS_SET_AGC_DECREMENT:
      st->max_decrease_step = exp(0.11513f * (*(ace_int32_t*)ptr)*st->frame_size / st->sampling_rate);
      break;
   case ACE_PREPROCESS_GET_AGC_DECREMENT:
      (*(ace_int32_t*)ptr) = floor(.5+8.6858*log(st->max_decrease_step)*st->sampling_rate/st->frame_size);
      break;
   case ACE_PREPROCESS_SET_AGC_MAX_GAIN:
      st->max_gain = exp(0.11513f * (*(ace_int32_t*)ptr));
      break;
   case ACE_PREPROCESS_GET_AGC_MAX_GAIN:
      (*(ace_int32_t*)ptr) = floor(.5+8.6858*log(st->max_gain));
      break;
#endif
   case ACE_PREPROCESS_SET_VAD:
      ace_warning("The VAD has been replaced by a hack pending a complete rewrite");
      st->vad_enabled = (*(ace_int32_t*)ptr);
      break;
   case ACE_PREPROCESS_GET_VAD:
      (*(ace_int32_t*)ptr) = st->vad_enabled;
      break;
   
   case ACE_PREPROCESS_SET_DEREVERB:
      st->dereverb_enabled = (*(ace_int32_t*)ptr);
      for (i=0;i<st->ps_size;i++)
         st->reverb_estimate[i]=0;
      break;
   case ACE_PREPROCESS_GET_DEREVERB:
      (*(ace_int32_t*)ptr) = st->dereverb_enabled;
      break;

   case ACE_PREPROCESS_SET_DEREVERB_LEVEL:
      /* FIXME: Re-enable when de-reverberation is actually enabled again */
      /*st->reverb_level = (*(float*)ptr);*/
      break;
   case ACE_PREPROCESS_GET_DEREVERB_LEVEL:
      /* FIXME: Re-enable when de-reverberation is actually enabled again */
      /*(*(float*)ptr) = st->reverb_level;*/
      break;
   
   case ACE_PREPROCESS_SET_DEREVERB_DECAY:
      /* FIXME: Re-enable when de-reverberation is actually enabled again */
      /*st->reverb_decay = (*(float*)ptr);*/
      break;
   case ACE_PREPROCESS_GET_DEREVERB_DECAY:
      /* FIXME: Re-enable when de-reverberation is actually enabled again */
      /*(*(float*)ptr) = st->reverb_decay;*/
      break;

   case ACE_PREPROCESS_SET_PROB_START:
      *(ace_int32_t*)ptr = MIN32(100,MAX32(0, *(ace_int32_t*)ptr));
      st->speech_prob_start = DIV32_16(MULT16_16(Q15ONE,*(ace_int32_t*)ptr), 100);
      break;
   case ACE_PREPROCESS_GET_PROB_START:
      (*(ace_int32_t*)ptr) = MULT16_16_Q15(st->speech_prob_start, 100);
      break;

   case ACE_PREPROCESS_SET_PROB_CONTINUE:
      *(ace_int32_t*)ptr = MIN32(100,MAX32(0, *(ace_int32_t*)ptr));
      st->speech_prob_continue = DIV32_16(MULT16_16(Q15ONE,*(ace_int32_t*)ptr), 100);
      break;
   case ACE_PREPROCESS_GET_PROB_CONTINUE:
      (*(ace_int32_t*)ptr) = MULT16_16_Q15(st->speech_prob_continue, 100);
      break;

   case ACE_PREPROCESS_SET_NOISE_SUPPRESS:
      st->noise_suppress = -ABS(*(ace_int32_t*)ptr);
      break;
   case ACE_PREPROCESS_GET_NOISE_SUPPRESS:
      (*(ace_int32_t*)ptr) = st->noise_suppress;
      break;
   case ACE_PREPROCESS_SET_ECHO_SUPPRESS:
      st->echo_suppress = -ABS(*(ace_int32_t*)ptr);
      break;
   case ACE_PREPROCESS_GET_ECHO_SUPPRESS:
      (*(ace_int32_t*)ptr) = st->echo_suppress;
      break;
   case ACE_PREPROCESS_SET_ECHO_SUPPRESS_ACTIVE:
      st->echo_suppress_active = -ABS(*(ace_int32_t*)ptr);
      break;
   case ACE_PREPROCESS_GET_ECHO_SUPPRESS_ACTIVE:
      (*(ace_int32_t*)ptr) = st->echo_suppress_active;
      break;
   case ACE_PREPROCESS_SET_ECHO_STATE:
      st->echo_state = (ACEEchoState*)ptr;
      break;
   case ACE_PREPROCESS_GET_ECHO_STATE:
      ptr = (void*)st->echo_state;
      break;
#ifndef FIXED_POINT
   case ACE_PREPROCESS_GET_AGC_LOUDNESS:
      (*(ace_int32_t*)ptr) = pow(st->loudness, 1.0/LOUDNESS_EXP);
      break;
#endif

   default:
      ace_warning_int("Unknown ace_preprocess_ctl request: ", request);
      return -1;
   }
   return 0;
}
