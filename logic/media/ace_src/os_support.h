#ifndef OS_SUPPORT_H
#define OS_SUPPORT_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ace_config.h"

#ifdef OS_SUPPORT_CUSTOM
#include "os_support_custom.h"
#endif

/** ACE wrapper for calloc. To do your own dynamic allocation, all you need to do is replace this function, ace_realloc and ace_free 
    NOTE: ace_alloc needs to CLEAR THE MEMORY */
#ifndef OVERRIDE_ACE_ALLOC
static inline void *ace_alloc (int size)
{
   /* WARNING: this is not equivalent to malloc(). If you want to use malloc() 
      or your own allocator, YOU NEED TO CLEAR THE MEMORY ALLOCATED. Otherwise
      you will experience strange bugs */
   return calloc(size,1);
}
#endif

/** Same as ace_alloc, except that the area is only needed inside a ACE call (might cause problem with wideband though) */
#ifndef OVERRIDE_ACE_ALLOC_SCRATCH
static inline void *ace_alloc_scratch (int size)
{
   /* Scratch space doesn't need to be cleared */
   return calloc(size,1);
}
#endif

/** ACE wrapper for realloc. To do your own dynamic allocation, all you need to do is replace this function, ace_alloc and ace_free */
#ifndef OVERRIDE_ACE_REALLOC
static inline void *ace_realloc (void *ptr, int size)
{
   return realloc(ptr, size);
}
#endif

/** ACE wrapper for calloc. To do your own dynamic allocation, all you need to do is replace this function, ace_realloc and ace_alloc */
#ifndef OVERRIDE_ACE_FREE
static inline void ace_free (void *ptr)
{
   free(ptr);
}
#endif

/** Same as ace_free, except that the area is only needed inside a ACE call (might cause problem with wideband though) */
#ifndef OVERRIDE_ACE_FREE_SCRATCH
static inline void ace_free_scratch (void *ptr)
{
   free(ptr);
}
#endif

/** Copy n bytes of memory from src to dst. The 0* term provides compile-time type checking  */
#ifndef OVERRIDE_ACE_COPY
#define ACE_COPY(dst, src, n) (memcpy((dst), (src), (n)*sizeof(*(dst)) + 0*((dst)-(src)) ))
#endif

/** Copy n bytes of memory from src to dst, allowing overlapping regions. The 0* term 
    provides compile-time type checking */
#ifndef OVERRIDE_ACE_MOVE
#define ACE_MOVE(dst, src, n) (memmove((dst), (src), (n)*sizeof(*(dst)) + 0*((dst)-(src)) ))
#endif

/** Set n bytes of memory to value of c, starting at address s */
#ifndef OVERRIDE_ACE_memset1
#define ACE_memset1(dst, c, n) (memset((dst), (c), (n)*sizeof(*(dst))))
#endif


#ifndef OVERRIDE_ACE_FATAL
static inline void _ace_fatal(const char *str, const char *file, int line)
{
   fprintf (stderr, "Fatal (internal) error in %s, line %d: %s\n", file, line, str);
   exit(1);
}
#endif

#ifndef OVERRIDE_ACE_WARNING
static inline void ace_warning(const char *str)
{
#ifndef DISABLE_WARNINGS
   fprintf (stderr, "warning: %s\n", str);
#endif
}
#endif

#ifndef OVERRIDE_ACE_WARNING_INT
static inline void ace_warning_int(const char *str, int val)
{
#ifndef DISABLE_WARNINGS
   fprintf (stderr, "warning: %s %d\n", str, val);
#endif
}
#endif

#ifndef OVERRIDE_ACE_NOTIFY
static inline void ace_notify(const char *str)
{
#ifndef DISABLE_NOTIFICATIONS
   fprintf (stderr, "notification: %s\n", str);
#endif
}
#endif

#ifndef OVERRIDE_ACE_PUTC
/** ACE wrapper for putc */
static inline void _ace_putc(int ch, void *file)
{
   FILE *f = (FILE *)file;
   fprintf(f, "%c", ch);
}
#endif

#define ace_fatal(str) _ace_fatal(str, __FILE__, __LINE__);
#define ace_assert(cond) {if (!(cond)) {ace_fatal("assertion failed: " #cond);}}

#ifndef RELEASE
static inline void print_vec(float *vec, int len, char *name)
{
   int i;
   printf ("%s ", name);
   for (i=0;i<len;i++)
      printf (" %f", vec[i]);
   printf ("\n");
}
#endif

#endif

