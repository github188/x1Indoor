#ifndef FILTERBANK_H
#define FILTERBANK_H

#include "ace_arch.h"

typedef struct {
   int *bank_left;
   int *bank_right;
   ace_word16_t *filter_left;
   ace_word16_t *filter_right;
#ifndef FIXED_POINT
   float *scaling;
#endif
   int nb_banks;
   int len;
} FilterBank;


FilterBank *filterbank_new(int banks, ace_word32_t sampling, int len, int type);

void filterbank_destroy(FilterBank *bank);

void filterbank_compute_bank32(FilterBank *bank, ace_word32_t *ps, ace_word32_t *mel);

void filterbank_compute_psd16(FilterBank *bank, ace_word16_t *mel, ace_word16_t *psd);

#ifndef FIXED_POINT
void filterbank_compute_bank(FilterBank *bank, float *psd, float *mel);
void filterbank_compute_psd(FilterBank *bank, float *mel, float *psd);
#endif


#endif
