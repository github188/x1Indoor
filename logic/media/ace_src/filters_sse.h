#include <xmmintrin.h>

void filter_mem16_10(const float *x, const float *_num, const float *_den, float *y, int N, int ord, float *_mem)
{
   __m128 num[3], den[3], mem[3];

   int i;

   /* Copy numerator, denominator and memory to aligned xmm */
   for (i=0;i<2;i++)
   {
      mem[i] = _mm_loadu_ps(_mem+4*i);
      num[i] = _mm_loadu_ps(_num+4*i);
      den[i] = _mm_loadu_ps(_den+4*i);
   }
   mem[2] = _mm_setr_ps(_mem[8], _mem[9], 0, 0);
   num[2] = _mm_setr_ps(_num[8], _num[9], 0, 0);
   den[2] = _mm_setr_ps(_den[8], _den[9], 0, 0);
   
   for (i=0;i<N;i++)
   {
      __m128 xx;
      __m128 yy;
      /* Compute next filter result */
      xx = _mm_load_ps1(x+i);
      yy = _mm_add_ss(xx, mem[0]);
      _mm_store_ss(y+i, yy);
      yy = _mm_shuffle_ps(yy, yy, 0);
      
      /* Update memory */
      mem[0] = _mm_move_ss(mem[0], mem[1]);
      mem[0] = _mm_shuffle_ps(mem[0], mem[0], 0x39);

      mem[0] = _mm_add_ps(mem[0], _mm_mul_ps(xx, num[0]));
      mem[0] = _mm_sub_ps(mem[0], _mm_mul_ps(yy, den[0]));

      mem[1] = _mm_move_ss(mem[1], mem[2]);
      mem[1] = _mm_shuffle_ps(mem[1], mem[1], 0x39);

      mem[1] = _mm_add_ps(mem[1], _mm_mul_ps(xx, num[1]));
      mem[1] = _mm_sub_ps(mem[1], _mm_mul_ps(yy, den[1]));

      mem[2] = _mm_shuffle_ps(mem[2], mem[2], 0xfd);

      mem[2] = _mm_add_ps(mem[2], _mm_mul_ps(xx, num[2]));
      mem[2] = _mm_sub_ps(mem[2], _mm_mul_ps(yy, den[2]));
   }
   /* Put memory back in its place */
   _mm_storeu_ps(_mem, mem[0]);
   _mm_storeu_ps(_mem+4, mem[1]);
   _mm_store_ss(_mem+8, mem[2]);
   mem[2] = _mm_shuffle_ps(mem[2], mem[2], 0x55);
   _mm_store_ss(_mem+9, mem[2]);
}

void filter_mem16_8(const float *x, const float *_num, const float *_den, float *y, int N, int ord, float *_mem)
{
   __m128 num[2], den[2], mem[2];

   int i;

   /* Copy numerator, denominator and memory to aligned xmm */
   for (i=0;i<2;i++)
   {
      mem[i] = _mm_loadu_ps(_mem+4*i);
      num[i] = _mm_loadu_ps(_num+4*i);
      den[i] = _mm_loadu_ps(_den+4*i);
   }
   
   for (i=0;i<N;i++)
   {
      __m128 xx;
      __m128 yy;
      /* Compute next filter result */
      xx = _mm_load_ps1(x+i);
      yy = _mm_add_ss(xx, mem[0]);
      _mm_store_ss(y+i, yy);
      yy = _mm_shuffle_ps(yy, yy, 0);
      
      /* Update memory */
      mem[0] = _mm_move_ss(mem[0], mem[1]);
      mem[0] = _mm_shuffle_ps(mem[0], mem[0], 0x39);

      mem[0] = _mm_add_ps(mem[0], _mm_mul_ps(xx, num[0]));
      mem[0] = _mm_sub_ps(mem[0], _mm_mul_ps(yy, den[0]));

      mem[1] = _mm_sub_ss(mem[1], mem[1]);
      mem[1] = _mm_shuffle_ps(mem[1], mem[1], 0x39);

      mem[1] = _mm_add_ps(mem[1], _mm_mul_ps(xx, num[1]));
      mem[1] = _mm_sub_ps(mem[1], _mm_mul_ps(yy, den[1]));
   }
   /* Put memory back in its place */
   _mm_storeu_ps(_mem, mem[0]);
   _mm_storeu_ps(_mem+4, mem[1]);
}


#define OVERRIDE_FILTER_MEM16
void filter_mem16(const float *x, const float *_num, const float *_den, float *y, int N, int ord, float *_mem, char *stack)
{
   if(ord==10)
      filter_mem16_10(x, _num, _den, y, N, ord, _mem);
   else if (ord==8)
      filter_mem16_8(x, _num, _den, y, N, ord, _mem);
}



void iir_mem16_10(const float *x, const float *_den, float *y, int N, int ord, float *_mem)
{
   __m128 den[3], mem[3];

   int i;

   /* Copy numerator, denominator and memory to aligned xmm */
   for (i=0;i<2;i++)
   {
      mem[i] = _mm_loadu_ps(_mem+4*i);
      den[i] = _mm_loadu_ps(_den+4*i);
   }
   mem[2] = _mm_setr_ps(_mem[8], _mem[9], 0, 0);
   den[2] = _mm_setr_ps(_den[8], _den[9], 0, 0);
   
   for (i=0;i<N;i++)
   {
      __m128 xx;
      __m128 yy;
      /* Compute next filter result */
      xx = _mm_load_ps1(x+i);
      yy = _mm_add_ss(xx, mem[0]);
      _mm_store_ss(y+i, yy);
      yy = _mm_shuffle_ps(yy, yy, 0);
      
      /* Update memory */
      mem[0] = _mm_move_ss(mem[0], mem[1]);
      mem[0] = _mm_shuffle_ps(mem[0], mem[0], 0x39);

      mem[0] = _mm_sub_ps(mem[0], _mm_mul_ps(yy, den[0]));

      mem[1] = _mm_move_ss(mem[1], mem[2]);
      mem[1] = _mm_shuffle_ps(mem[1], mem[1], 0x39);

      mem[1] = _mm_sub_ps(mem[1], _mm_mul_ps(yy, den[1]));

      mem[2] = _mm_shuffle_ps(mem[2], mem[2], 0xfd);

      mem[2] = _mm_sub_ps(mem[2], _mm_mul_ps(yy, den[2]));
   }
   /* Put memory back in its place */
   _mm_storeu_ps(_mem, mem[0]);
   _mm_storeu_ps(_mem+4, mem[1]);
   _mm_store_ss(_mem+8, mem[2]);
   mem[2] = _mm_shuffle_ps(mem[2], mem[2], 0x55);
   _mm_store_ss(_mem+9, mem[2]);
}


void iir_mem16_8(const float *x, const float *_den, float *y, int N, int ord, float *_mem)
{
   __m128 den[2], mem[2];

   int i;

   /* Copy numerator, denominator and memory to aligned xmm */
   for (i=0;i<2;i++)
   {
      mem[i] = _mm_loadu_ps(_mem+4*i);
      den[i] = _mm_loadu_ps(_den+4*i);
   }
   
   for (i=0;i<N;i++)
   {
      __m128 xx;
      __m128 yy;
      /* Compute next filter result */
      xx = _mm_load_ps1(x+i);
      yy = _mm_add_ss(xx, mem[0]);
      _mm_store_ss(y+i, yy);
      yy = _mm_shuffle_ps(yy, yy, 0);
      
      /* Update memory */
      mem[0] = _mm_move_ss(mem[0], mem[1]);
      mem[0] = _mm_shuffle_ps(mem[0], mem[0], 0x39);

      mem[0] = _mm_sub_ps(mem[0], _mm_mul_ps(yy, den[0]));

      mem[1] = _mm_sub_ss(mem[1], mem[1]);
      mem[1] = _mm_shuffle_ps(mem[1], mem[1], 0x39);

      mem[1] = _mm_sub_ps(mem[1], _mm_mul_ps(yy, den[1]));
   }
   /* Put memory back in its place */
   _mm_storeu_ps(_mem, mem[0]);
   _mm_storeu_ps(_mem+4, mem[1]);
}

#define OVERRIDE_IIR_MEM16
void iir_mem16(const float *x, const float *_den, float *y, int N, int ord, float *_mem, char *stack)
{
   if(ord==10)
      iir_mem16_10(x, _den, y, N, ord, _mem);
   else if (ord==8)
      iir_mem16_8(x, _den, y, N, ord, _mem);
}


void fir_mem16_10(const float *x, const float *_num, float *y, int N, int ord, float *_mem)
{
   __m128 num[3], mem[3];

   int i;

   /* Copy numerator, denominator and memory to aligned xmm */
   for (i=0;i<2;i++)
   {
      mem[i] = _mm_loadu_ps(_mem+4*i);
      num[i] = _mm_loadu_ps(_num+4*i);
   }
   mem[2] = _mm_setr_ps(_mem[8], _mem[9], 0, 0);
   num[2] = _mm_setr_ps(_num[8], _num[9], 0, 0);
   
   for (i=0;i<N;i++)
   {
      __m128 xx;
      __m128 yy;
      /* Compute next filter result */
      xx = _mm_load_ps1(x+i);
      yy = _mm_add_ss(xx, mem[0]);
      _mm_store_ss(y+i, yy);
      yy = _mm_shuffle_ps(yy, yy, 0);
      
      /* Update memory */
      mem[0] = _mm_move_ss(mem[0], mem[1]);
      mem[0] = _mm_shuffle_ps(mem[0], mem[0], 0x39);

      mem[0] = _mm_add_ps(mem[0], _mm_mul_ps(xx, num[0]));

      mem[1] = _mm_move_ss(mem[1], mem[2]);
      mem[1] = _mm_shuffle_ps(mem[1], mem[1], 0x39);

      mem[1] = _mm_add_ps(mem[1], _mm_mul_ps(xx, num[1]));

      mem[2] = _mm_shuffle_ps(mem[2], mem[2], 0xfd);

      mem[2] = _mm_add_ps(mem[2], _mm_mul_ps(xx, num[2]));
   }
   /* Put memory back in its place */
   _mm_storeu_ps(_mem, mem[0]);
   _mm_storeu_ps(_mem+4, mem[1]);
   _mm_store_ss(_mem+8, mem[2]);
   mem[2] = _mm_shuffle_ps(mem[2], mem[2], 0x55);
   _mm_store_ss(_mem+9, mem[2]);
}

void fir_mem16_8(const float *x, const float *_num, float *y, int N, int ord, float *_mem)
{
   __m128 num[2], mem[2];

   int i;

   /* Copy numerator, denominator and memory to aligned xmm */
   for (i=0;i<2;i++)
   {
      mem[i] = _mm_loadu_ps(_mem+4*i);
      num[i] = _mm_loadu_ps(_num+4*i);
   }
   
   for (i=0;i<N;i++)
   {
      __m128 xx;
      __m128 yy;
      /* Compute next filter result */
      xx = _mm_load_ps1(x+i);
      yy = _mm_add_ss(xx, mem[0]);
      _mm_store_ss(y+i, yy);
      yy = _mm_shuffle_ps(yy, yy, 0);
      
      /* Update memory */
      mem[0] = _mm_move_ss(mem[0], mem[1]);
      mem[0] = _mm_shuffle_ps(mem[0], mem[0], 0x39);

      mem[0] = _mm_add_ps(mem[0], _mm_mul_ps(xx, num[0]));

      mem[1] = _mm_sub_ss(mem[1], mem[1]);
      mem[1] = _mm_shuffle_ps(mem[1], mem[1], 0x39);

      mem[1] = _mm_add_ps(mem[1], _mm_mul_ps(xx, num[1]));
   }
   /* Put memory back in its place */
   _mm_storeu_ps(_mem, mem[0]);
   _mm_storeu_ps(_mem+4, mem[1]);
}

#define OVERRIDE_FIR_MEM16
void fir_mem16(const float *x, const float *_num, float *y, int N, int ord, float *_mem, char *stack)
{
   if(ord==10)
      fir_mem16_10(x, _num, y, N, ord, _mem);
   else if (ord==8)
      fir_mem16_8(x, _num, y, N, ord, _mem);
}
