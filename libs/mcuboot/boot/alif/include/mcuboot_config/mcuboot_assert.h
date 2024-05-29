#ifndef MCUBOOT_ASSERT_H
#define MCUBOOT_ASSERT_H

#ifdef NDEBUG

// default C library assert just does nothing on NDEBUG.
// this causes mcuboot library to emit unused variable warnings
// this define silences the warnings so it can be built with
// -Werror

#define assert(x) \
   do {           \
   (void)(x);     \
} while (0)       \

#else
#include <assert.h>
#endif

#endif
