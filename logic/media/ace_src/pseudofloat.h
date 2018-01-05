#ifndef PSEUDOFLOAT_H
#define PSEUDOFLOAT_H

#include "ace_arch.h"
#include "os_support.h"
#include "math_approx.h"
#include <math.h>

#ifdef FIXED_POINT

typedef struct {
   ace_int16_t m;
   ace_int16_t e;
} ace_float_t;

static const ace_float_t FLOAT_ZERO = {0,0};
static const ace_float_t FLOAT_ONE = {16384,-14};
static const ace_float_t FLOAT_HALF = {16384,-15};

#define MIN(a,b) ((a)<(b)?(a):(b))
static inline ace_float_t PSEUDOFLOAT(ace_int32_t x)
{
   int e=0;
   int sign=0;
   if (x<0)
   {
      sign = 1;
      x = -x;
   }
   if (x==0)
   {
      ace_float_t r = {0,0};
      return r;
   }
   e = ace_ilog2(ABS32(x))-14;
   x = VSHR32(x, e);
   if (sign)
   {
      ace_float_t r;
      r.m = -x;
      r.e = e;
      return r;
   }
   else      
   {
      ace_float_t r;
      r.m = x;
      r.e = e;
      return r;
   }
}


static inline ace_float_t FLOAT_ADD(ace_float_t a, ace_float_t b)
{
   ace_float_t r;
   if (a.m==0)
      return b;
   else if (b.m==0)
      return a;
   if ((a).e > (b).e) 
   {
      r.m = ((a).m>>1) + ((b).m>>MIN(15,(a).e-(b).e+1));
      r.e = (a).e+1;
   }
   else 
   {
      r.m = ((b).m>>1) + ((a).m>>MIN(15,(b).e-(a).e+1));
      r.e = (b).e+1;
   }
   if (r.m>0)
   {
      if (r.m<16384)
      {
         r.m<<=1;
         r.e-=1;
      }
   } else {
      if (r.m>-16384)
      {
         r.m<<=1;
         r.e-=1;
      }
   }
   /*printf ("%f + %f = %f\n", REALFLOAT(a), REALFLOAT(b), REALFLOAT(r));*/
   return r;
}

static inline ace_float_t FLOAT_SUB(ace_float_t a, ace_float_t b)
{
   ace_float_t r;
   if (a.m==0)
      return b;
   else if (b.m==0)
      return a;
   if ((a).e > (b).e)
   {
      r.m = ((a).m>>1) - ((b).m>>MIN(15,(a).e-(b).e+1));
      r.e = (a).e+1;
   }
   else 
   {
      r.m = ((a).m>>MIN(15,(b).e-(a).e+1)) - ((b).m>>1);
      r.e = (b).e+1;
   }
   if (r.m>0)
   {
      if (r.m<16384)
      {
         r.m<<=1;
         r.e-=1;
      }
   } else {
      if (r.m>-16384)
      {
         r.m<<=1;
         r.e-=1;
      }
   }
   /*printf ("%f + %f = %f\n", REALFLOAT(a), REALFLOAT(b), REALFLOAT(r));*/
   return r;
}

static inline int FLOAT_LT(ace_float_t a, ace_float_t b)
{
   if (a.m==0)
      return b.m>0;
   else if (b.m==0)
      return a.m<0;   
   if ((a).e > (b).e)
      return ((a).m>>1) < ((b).m>>MIN(15,(a).e-(b).e+1));
   else 
      return ((b).m>>1) > ((a).m>>MIN(15,(b).e-(a).e+1));

}

static inline int FLOAT_GT(ace_float_t a, ace_float_t b)
{
   return FLOAT_LT(b,a);
}

static inline ace_float_t FLOAT_MULT(ace_float_t a, ace_float_t b)
{
   ace_float_t r;
   r.m = (ace_int16_t)((ace_int32_t)(a).m*(b).m>>15);
   r.e = (a).e+(b).e+15;
   if (r.m>0)
   {
      if (r.m<16384)
      {
         r.m<<=1;
         r.e-=1;
      }
   } else {
      if (r.m>-16384)
      {
         r.m<<=1;
         r.e-=1;
      }
   }
   /*printf ("%f * %f = %f\n", REALFLOAT(a), REALFLOAT(b), REALFLOAT(r));*/
   return r;   
}

static inline ace_float_t FLOAT_AMULT(ace_float_t a, ace_float_t b)
{
   ace_float_t r;
   r.m = (ace_int16_t)((ace_int32_t)(a).m*(b).m>>15);
   r.e = (a).e+(b).e+15;
   return r;   
}


static inline ace_float_t FLOAT_SHL(ace_float_t a, int b)
{
   ace_float_t r;
   r.m = a.m;
   r.e = a.e+b;
   return r;
}

static inline ace_int16_t FLOAT_EXTRACT16(ace_float_t a)
{
   if (a.e<0)
      return EXTRACT16((EXTEND32(a.m)+(EXTEND32(1)<<(-a.e-1)))>>-a.e);
   else
      return a.m<<a.e;
}

static inline ace_int32_t FLOAT_EXTRACT32(ace_float_t a)
{
   if (a.e<0)
      return (EXTEND32(a.m)+(EXTEND32(1)<<(-a.e-1)))>>-a.e;
   else
      return EXTEND32(a.m)<<a.e;
}

static inline ace_int32_t FLOAT_MUL32(ace_float_t a, ace_word32_t b)
{
   return VSHR32(MULT16_32_Q15(a.m, b),-a.e-15);
}

static inline ace_float_t FLOAT_MUL32U(ace_word32_t a, ace_word32_t b)
{
   int e1, e2;
   ace_float_t r;
   if (a==0 || b==0)
   {
      return FLOAT_ZERO;
   }
   e1 = ace_ilog2(ABS32(a));
   a = VSHR32(a, e1-14);
   e2 = ace_ilog2(ABS32(b));
   b = VSHR32(b, e2-14);
   r.m = MULT16_16_Q15(a,b);
   r.e = e1+e2-13;
   return r;
}

/* Do NOT attempt to divide by a negative number */
static inline ace_float_t FLOAT_DIV32_FLOAT(ace_word32_t a, ace_float_t b)
{
   int e=0;
   ace_float_t r;
   if (a==0)
   {
      return FLOAT_ZERO;
   }
   e = ace_ilog2(ABS32(a))-ace_ilog2(b.m-1)-15;
   a = VSHR32(a, e);
   if (ABS32(a)>=SHL32(EXTEND32(b.m-1),15))
   {
      a >>= 1;
      e++;
   }
   r.m = DIV32_16(a,b.m);
   r.e = e-b.e;
   return r;
}


/* Do NOT attempt to divide by a negative number */
static inline ace_float_t FLOAT_DIV32(ace_word32_t a, ace_word32_t b)
{
   int e0=0,e=0;
   ace_float_t r;
   if (a==0)
   {
      return FLOAT_ZERO;
   }
   if (b>32767)
   {
      e0 = ace_ilog2(b)-14;
      b = VSHR32(b, e0);
      e0 = -e0;
   }
   e = ace_ilog2(ABS32(a))-ace_ilog2(b-1)-15;
   a = VSHR32(a, e);
   if (ABS32(a)>=SHL32(EXTEND32(b-1),15))
   {
      a >>= 1;
      e++;
   }
   e += e0;
   r.m = DIV32_16(a,b);
   r.e = e;
   return r;
}

/* Do NOT attempt to divide by a negative number */
static inline ace_float_t FLOAT_DIVU(ace_float_t a, ace_float_t b)
{
   int e=0;
   ace_int32_t num;
   ace_float_t r;
   if (b.m<=0)
   {
      ace_warning_int("Attempted to divide by", b.m);
      return FLOAT_ONE;
   }
   num = a.m;
   a.m = ABS16(a.m);
   while (a.m >= b.m)
   {
      e++;
      a.m >>= 1;
   }
   num = num << (15-e);
   r.m = DIV32_16(num,b.m);
   r.e = a.e-b.e-15+e;
   return r;
}

static inline ace_float_t FLOAT_SQRT(ace_float_t a)
{
   ace_float_t r;
   ace_int32_t m;
   m = SHL32(EXTEND32(a.m), 14);
   r.e = a.e - 14;
   if (r.e & 1)
   {
      r.e -= 1;
      m <<= 1;
   }
   r.e >>= 1;
   r.m = ace_sqrt(m);
   return r;
}

#else

#define ace_float_t float
#define FLOAT_ZERO 0.f
#define FLOAT_ONE 1.f
#define FLOAT_HALF 0.5f
#define PSEUDOFLOAT(x) (x)
#define FLOAT_MULT(a,b) ((a)*(b))
#define FLOAT_AMULT(a,b) ((a)*(b))
#define FLOAT_MUL32(a,b) ((a)*(b))
#define FLOAT_DIV32(a,b) ((a)/(b))
#define FLOAT_EXTRACT16(a) (a)
#define FLOAT_EXTRACT32(a) (a)
#define FLOAT_ADD(a,b) ((a)+(b))
#define FLOAT_SUB(a,b) ((a)-(b))
#define REALFLOAT(x) (x)
#define FLOAT_DIV32_FLOAT(a,b) ((a)/(b))
#define FLOAT_MUL32U(a,b) ((a)*(b))
#define FLOAT_SHL(a,b) (a)
#define FLOAT_LT(a,b) ((a)<(b))
#define FLOAT_GT(a,b) ((a)>(b))
#define FLOAT_DIVU(a,b) ((a)/(b))
#define FLOAT_SQRT(a) (ace_sqrt(a))

#endif

#endif
