/**
   @file smallft.h
   @brief Discrete Rotational Fourier Transform (DRFT)
*/

#ifndef _V_SMFT_H_
#define _V_SMFT_H_


#ifdef __cplusplus
extern "C" {
#endif

/** Discrete Rotational Fourier Transform lookup */
struct drft_lookup{
  int n;
  float *trigcache;
  int *splitcache;
};

extern void ace_drft_forward(struct drft_lookup *l,float *data);
extern void ace_drft_backward(struct drft_lookup *l,float *data);
extern void ace_drft_init(struct drft_lookup *l,int n);
extern void ace_drft_clear(struct drft_lookup *l);

#ifdef __cplusplus
}
#endif

#endif
