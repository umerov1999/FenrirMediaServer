/* jconfig.vc --- jconfig.h for Microsoft Visual C++ on Windows 9x or NT.
 * This file also works for Borland/Embarcadero C++ for Win32 or Win64
 * (CLI: bcc32, bcc32c, bcc32x, bcc64; GUI IDE: C++Builder/RAD Studio).
 * See jconfig.txt for explanations.
 */

#define SYSDEPMEM jmenobs.c  /* No backing store. Use only memory */
#define HAVE_PROTOTYPES
#define HAVE_UNSIGNED_CHAR
#define HAVE_UNSIGNED_SHORT
/* #define void char */
/* #define const */
#undef CHAR_IS_UNSIGNED
#define HAVE_STDDEF_H
#define HAVE_STDLIB_H
#undef NEED_BSD_STRINGS
#undef NEED_SYS_TYPES_H
#undef NEED_FAR_POINTERS	/* we presume a 32-bit flat memory model */
#undef NEED_SHORT_EXTERNAL_NAMES
#undef INCOMPLETE_TYPES_BROKEN

/* Define "boolean" as unsigned char, not enum, per Windows custom */
#ifndef __RPCNDR_H__		/* don't conflict if rpcndr.h already read */
typedef unsigned char boolean;
#endif
#ifndef FALSE			/* in case these macros already exist */
#define FALSE	0		/* values of boolean */
#endif
#ifndef TRUE
#define TRUE	1
#endif
#define HAVE_BOOLEAN		/* prevent jmorecfg.h from redefining it */

/* Define custom RGB color order, prevent jmorecfg.h from redefinition */
#undef JPEG_HAVE_RGB_CUSTOM
/* Use Windows custom BGR color order defined in jmorecfg.h */
#undef JPEG_USE_RGB_CUSTOM

/* Define custom file I/O functions, prevent jinclude.h from redefinition */
#undef JPEG_HAVE_FILE_IO_CUSTOM
/* Use Delphi custom file I/O functions defined in jinclude.h */
#undef JPEG_USE_FILE_IO_CUSTOM


#ifdef JPEG_INTERNALS

#undef RIGHT_SHIFT_IS_UNSIGNED

#endif /* JPEG_INTERNALS */

#include "Windows.h"
#undef USE_WINDOWS_MESSAGEBOX
