#ifndef ECHO_WRAP_H
#define ECHO_WRAP_H


void echo_cancel_init(int framesize, int tail, int noiselevel, int echolevel);

//destroy echo cancel context, free memory
void echo_cancel_free(void);

//input 16bit linear PCM data of far_in(far_end speech) and near_in(near end speech)
//output PCM echo canceled data
void echo_cancel_exe( short *spk_buf,  short *echo_buf, short *out_buf);

#endif
