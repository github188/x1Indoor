#include "ace_config.h"

/*#define USE_SMALLFT*/
#define USE_KISS_FFT


#include "ace_arch.h"
#include "os_support.h"

#define MAX_FFT_SIZE 2048

#ifdef FIXED_POINT
static int maximize_range(ace_word16_t *in, ace_word16_t *out, ace_word16_t bound, int len)
{
   int i, shift;
   ace_word16_t max_val = 0;
   for (i=0;i<len;i++)
   {
      if (in[i]>max_val)
         max_val = in[i];
      if (-in[i]>max_val)
         max_val = -in[i];
   }
   shift=0;
   while (max_val <= (bound>>1) && max_val != 0)
   {
      max_val <<= 1;
      shift++;
   }
   for (i=0;i<len;i++)
   {
      out[i] = SHL16(in[i], shift);
   }   
   return shift;
}

static void renorm_range(ace_word16_t *in, ace_word16_t *out, int shift, int len)
{
   int i;
   for (i=0;i<len;i++)
   {
      out[i] = PSHR16(in[i], shift);
   }
}
#endif

#ifdef USE_SMALLFT

#include "smallft.h"
#include <math.h>

void *ace_fft_init(int size)
{
   struct drft_lookup *table;
   table = ace_alloc(sizeof(struct drft_lookup));
   ace_drft_init((struct drft_lookup *)table, size);
   return (void*)table;
}

void ace_fft_destroy(void *table)
{
   ace_drft_clear(table);
   ace_free(table);
}

void ace_fft(void *table, float *in, float *out)
{
   if (in==out)
   {
      int i;
      float scale = 1./((struct drft_lookup *)table)->n;
      ace_warning("FFT should not be done in-place");
      for (i=0;i<((struct drft_lookup *)table)->n;i++)
         out[i] = scale*in[i];
   } else {
      int i;
      float scale = 1./((struct drft_lookup *)table)->n;
      for (i=0;i<((struct drft_lookup *)table)->n;i++)
         out[i] = scale*in[i];
   }
   ace_drft_forward((struct drft_lookup *)table, out);
}

void ace_ifft(void *table, float *in, float *out)
{
   if (in==out)
   {
      ace_warning("FFT should not be done in-place");
   } else {
      int i;
      for (i=0;i<((struct drft_lookup *)table)->n;i++)
         out[i] = in[i];
   }
   ace_drft_backward((struct drft_lookup *)table, out);
}

#elif defined(USE_KISS_FFT)

#include "kiss_fftr.h"
#include "kiss_fft.h"

struct kiss_config {
   kiss_fftr_cfg forward;
   kiss_fftr_cfg backward;
   int N;
};

void *ace_fft_init(int size)
{
   struct kiss_config *table;
   table = (struct kiss_config*)ace_alloc(sizeof(struct kiss_config));
   table->forward = kiss_fftr_alloc(size,0,NULL,NULL);
   table->backward = kiss_fftr_alloc(size,1,NULL,NULL);
   table->N = size;
   return table;
}

void ace_fft_destroy(void *table)
{
   struct kiss_config *t = (struct kiss_config *)table;
   kiss_fftr_free(t->forward);
   kiss_fftr_free(t->backward);
   ace_free(table);
}

#ifdef FIXED_POINT

void ace_fft(void *table, ace_word16_t *in, ace_word16_t *out)
{
   int shift;
   struct kiss_config *t = (struct kiss_config *)table;
   shift = maximize_range(in, in, 32000, t->N);
   kiss_fftr2(t->forward, in, out);
   renorm_range(in, in, shift, t->N);
   renorm_range(out, out, shift, t->N);
}

#else

void ace_fft(void *table, ace_word16_t *in, ace_word16_t *out)
{
   int i;
   float scale;
   struct kiss_config *t = (struct kiss_config *)table;
   scale = 1./t->N;
   kiss_fftr2(t->forward, in, out);
   for (i=0;i<t->N;i++)
      out[i] *= scale;
}
#endif

void ace_ifft(void *table, ace_word16_t *in, ace_word16_t *out)
{
   struct kiss_config *t = (struct kiss_config *)table;
   kiss_fftri2(t->backward, in, out);
}


#else

#error No other FFT implemented

#endif


#ifdef FIXED_POINT
/*#include "smallft.h"*/


void ace_fft_float(void *table, float *in, float *out)
{
   int i;
#ifdef USE_SMALLFT
   int N = ((struct drft_lookup *)table)->n;
#elif defined(USE_KISS_FFT)
   int N = ((struct kiss_config *)table)->N;
#else
#endif
#ifdef VAR_ARRAYS
   ace_word16_t _in[N];
   ace_word16_t _out[N];
#else
   ace_word16_t _in[MAX_FFT_SIZE];
   ace_word16_t _out[MAX_FFT_SIZE];
#endif
   for (i=0;i<N;i++)
      _in[i] = (int)floor(.5+in[i]);
   ace_fft(table, _in, _out);
   for (i=0;i<N;i++)
      out[i] = _out[i];
#if 0
   if (!fixed_point)
   {
      float scale;
      struct drft_lookup t;
      ace_drft_init(&t, ((struct kiss_config *)table)->N);
      scale = 1./((struct kiss_config *)table)->N;
      for (i=0;i<((struct kiss_config *)table)->N;i++)
         out[i] = scale*in[i];
      ace_drft_forward(&t, out);
      ace_drft_clear(&t);
   }
#endif
}

void ace_ifft_float(void *table, float *in, float *out)
{
   int i;
#ifdef USE_SMALLFT
   int N = ((struct drft_lookup *)table)->n;
#elif defined(USE_KISS_FFT)
   int N = ((struct kiss_config *)table)->N;
#else
#endif
#ifdef VAR_ARRAYS
   ace_word16_t _in[N];
   ace_word16_t _out[N];
#else
   ace_word16_t _in[MAX_FFT_SIZE];
   ace_word16_t _out[MAX_FFT_SIZE];
#endif
   for (i=0;i<N;i++)
      _in[i] = (int)floor(.5+in[i]);
   ace_ifft(table, _in, _out);
   for (i=0;i<N;i++)
      out[i] = _out[i];
#if 0
   if (!fixed_point)
   {
      int i;
      struct drft_lookup t;
      ace_drft_init(&t, ((struct kiss_config *)table)->N);
      for (i=0;i<((struct kiss_config *)table)->N;i++)
         out[i] = in[i];
      ace_drft_backward(&t, out);
      ace_drft_clear(&t);
   }
#endif
}

#else

void ace_fft_float(void *table, float *in, float *out)
{
   ace_fft(table, in, out);
}
void ace_ifft_float(void *table, float *in, float *out)
{
   ace_ifft(table, in, out);
}

#endif
