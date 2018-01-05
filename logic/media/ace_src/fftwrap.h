#ifndef FFTWRAP_H
#define FFTWRAP_H

#include "ace_arch.h"

/** Compute tables for an FFT */
void *ace_fft_init(int size);

/** Destroy tables for an FFT */
void ace_fft_destroy(void *table);

/** Forward (real to half-complex) transform */
void ace_fft(void *table, ace_word16_t *in, ace_word16_t *out);

/** Backward (half-complex to real) transform */
void ace_ifft(void *table, ace_word16_t *in, ace_word16_t *out);

/** Forward (real to half-complex) transform of float data */
void ace_fft_float(void *table, float *in, float *out);

/** Backward (half-complex to real) transform of float data */
void ace_ifft_float(void *table, float *in, float *out);

#endif
