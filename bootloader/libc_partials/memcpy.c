#include <libc_partials/include/types.h>
#include <libc_partials/include/string.h>

#define MEMCPY_VERSION_NAME memcpy_from_volatile_to_volatile
#define MEMCPY_SRC_QUALIFIER_BEFORE const volatile
#define MEMCPY_SRC_QUALIFIER_AFTER restrict
#define MEMCPY_DST_QUALIFIER_BEFORE volatile
#define MEMCPY_DST_QUALIFIER_AFTER restrict
#define MEMCPY_RETURN_TYPE volatile void*

#include "memcpy.inc"
