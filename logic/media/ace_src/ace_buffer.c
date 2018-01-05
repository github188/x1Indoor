#include "ace_config.h"

#include "os_support.h"
#include "ace_arch.h"
#include "ace_buffer.h"

struct ACEBuffer_ {
   char *data;
   int   size;
   int   read_ptr;
   int   write_ptr;
   int   available;
};

ACEBuffer *ace_buffer_init(int size)
{
   ACEBuffer *st = ace_alloc(sizeof(ACEBuffer));
   st->data = ace_alloc(size);
   st->size = size;
   st->read_ptr = 0;
   st->write_ptr = 0;
   st->available = 0;
   return st;
}

void ace_buffer_destroy(ACEBuffer *st)
{
   ace_free(st->data);
   ace_free(st);
}

int ace_buffer_write(ACEBuffer *st, void *_data, int len)
{
   int end;
   int end1;
   char *data = _data;
   if (len > st->size)
   {
      data += len-st->size;
      len = st->size;
   }
   end = st->write_ptr + len;
   end1 = end;
   if (end1 > st->size)
      end1 = st->size;
   ACE_COPY(st->data + st->write_ptr, data, end1 - st->write_ptr);
   if (end > st->size)
   {
      end -= st->size;
      ACE_COPY(st->data, data+end1 - st->write_ptr, end);
   }
   st->available += len;
   if (st->available > st->size)
   {
      st->available = st->size;
      st->read_ptr = st->write_ptr;
   }
   st->write_ptr += len;
   if (st->write_ptr > st->size)
      st->write_ptr -= st->size;
   return len;
}

int ace_buffer_writezeros(ACEBuffer *st, int len)
{
   /* This is almost the same as for ace_buffer_write() but using 
   ACE_memset1() instead of ACE_COPY(). Update accordingly. */
   int end;
   int end1;
   if (len > st->size)
   {
      len = st->size;
   }
   end = st->write_ptr + len;
   end1 = end;
   if (end1 > st->size)
      end1 = st->size;
   ACE_memset1(st->data + st->write_ptr, 0, end1 - st->write_ptr);
   if (end > st->size)
   {
      end -= st->size;
      ACE_memset1(st->data, 0, end);
   }
   st->available += len;
   if (st->available > st->size)
   {
      st->available = st->size;
      st->read_ptr = st->write_ptr;
   }
   st->write_ptr += len;
   if (st->write_ptr > st->size)
      st->write_ptr -= st->size;
   return len;
}

int ace_buffer_read(ACEBuffer *st, void *_data, int len)
{
   int end, end1;
   char *data = _data;
   if (len > st->available)
   {
      ACE_memset1(data+st->available, 0, st->size-st->available);
      len = st->available;
   }
   end = st->read_ptr + len;
   end1 = end;
   if (end1 > st->size)
      end1 = st->size;
   ACE_COPY(data, st->data + st->read_ptr, end1 - st->read_ptr);

   if (end > st->size)
   {
      end -= st->size;
      ACE_COPY(data+end1 - st->read_ptr, st->data, end);
   }
   st->available -= len;
   st->read_ptr += len;
   if (st->read_ptr > st->size)
      st->read_ptr -= st->size;
   return len;
}

int ace_buffer_get_available(ACEBuffer *st)
{
   return st->available;
}

int ace_buffer_resize(ACEBuffer *st, int len)
{
   int old_len = st->size;
   if (len > old_len)
   {
      st->data = ace_realloc(st->data, len);
      /* FIXME: move data/pointers properly for growing the buffer */
   } else {
      /* FIXME: move data/pointers properly for shrinking the buffer */
      st->data = ace_realloc(st->data, len);
   }
   return len;
}
