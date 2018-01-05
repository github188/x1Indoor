#ifndef ACE_BUFFER_H
#define ACE_BUFFER_H

#include "ace_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ACEBuffer_;
typedef struct ACEBuffer_ ACEBuffer;

ACEBuffer *ace_buffer_init(int size);

void ace_buffer_destroy(ACEBuffer *st);

int ace_buffer_write(ACEBuffer *st, void *data, int len);

int ace_buffer_writezeros(ACEBuffer *st, int len);

int ace_buffer_read(ACEBuffer *st, void *data, int len);

int ace_buffer_get_available(ACEBuffer *st);

int ace_buffer_resize(ACEBuffer *st, int len);

#ifdef __cplusplus
}
#endif

#endif




