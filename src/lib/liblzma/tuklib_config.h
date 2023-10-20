#ifndef HAVE_CONFIG_H
#define HAVE_CONFIG_H
#endif
// If config.h isn't available, assume that the headers required by
// tuklib_common.h are available. This is required by crc32_tablegen.c.
#ifdef HAVE_CONFIG_H
#	include "lib/liblzma/sysdefs.h"
#else
#	include <stddef.h>
#	include <stdbool.h>
#	include <inttypes.h>
#	include <limits.h>
#endif
