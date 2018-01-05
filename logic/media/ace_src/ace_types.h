/**
   @file ace_types.h
   @brief ace types
*/
#ifndef _ACE_TYPES_H
#define _ACE_TYPES_H

#if defined(_WIN32) 

#  if defined(__CYGWIN__)
#    include <_G_config.h>
     typedef _G_int32_t ace_int32_t;
     typedef _G_uint32_t ace_uint32_t;
     typedef _G_int16_t ace_int16_t;
     typedef _G_uint16_t ace_uint16_t;
#  elif defined(__MINGW32__)
     typedef short ace_int16_t;
     typedef unsigned short ace_uint16_t;
     typedef int ace_int32_t;
     typedef unsigned int ace_uint32_t;
#  elif defined(__MWERKS__)
     typedef int ace_int32_t;
     typedef unsigned int ace_uint32_t;
     typedef short ace_int16_t;
     typedef unsigned short ace_uint16_t;
#  else
     /* MSVC/Borland */
     typedef __int32 ace_int32_t;
     typedef unsigned __int32 ace_uint32_t;
     typedef __int16 ace_int16_t;
     typedef unsigned __int16 ace_uint16_t;
#  endif

#elif defined(__MACOS__)

#  include <sys/types.h>
   typedef SInt16 ace_int16_t;
   typedef UInt16 ace_uint16_t;
   typedef SInt32 ace_int32_t;
   typedef UInt32 ace_uint32_t;

#elif (defined(__APPLE__) && defined(__MACH__)) /* MacOS X Framework build */

#  include <sys/types.h>
   typedef int16_t ace_int16_t;
   typedef u_int16_t ace_uint16_t;
   typedef int32_t ace_int32_t;
   typedef u_int32_t ace_uint32_t;

#elif defined(__BEOS__)

   /* Be */
#  include <inttypes.h>
   typedef int16_t ace_int16_t;
   typedef u_int16_t ace_uint16_t;
   typedef int32_t ace_int32_t;
   typedef u_int32_t ace_uint32_t;

#elif defined (__EMX__)

   /* OS/2 GCC */
   typedef short ace_int16_t;
   typedef unsigned short ace_uint16_t;
   typedef int ace_int32_t;
   typedef unsigned int ace_uint32_t;

#elif defined (DJGPP)

   /* DJGPP */
   typedef short ace_int16_t;
   typedef int ace_int32_t;
   typedef unsigned int ace_uint32_t;

#elif defined(R5900)

   /* PS2 EE */
   typedef int ace_int32_t;
   typedef unsigned ace_uint32_t;
   typedef short ace_int16_t;

#elif defined(__SYMBIAN32__)

   /* Symbian GCC */
   typedef signed short ace_int16_t;
   typedef unsigned short ace_uint16_t;
   typedef signed int ace_int32_t;
   typedef unsigned int ace_uint32_t;

#elif defined(CONFIG_TI_C54X) || defined (CONFIG_TI_C55X)

   typedef short ace_int16_t;
   typedef unsigned short ace_uint16_t;
   typedef long ace_int32_t;
   typedef unsigned long ace_uint32_t;

#elif defined(CONFIG_TI_C6X)

   typedef short ace_int16_t;
   typedef unsigned short ace_uint16_t;
   typedef int ace_int32_t;
   typedef unsigned int ace_uint32_t;

#else

#include "ace_config_types.h"

#endif

#endif  /* _ACE_TYPES_H */
