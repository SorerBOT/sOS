#include <types.h>
#include <string.h>

#define MEMCPY_VERSION_NAME memcpy
#define MEMCPY_SRC_QUALIFIER_BEFORE const
#define MEMCPY_SRC_QUALIFIER_AFTER restrict
#define MEMCPY_DST_QUALIFIER_BEFORE
#define MEMCPY_DST_QUALIFIER_AFTER restrict
#define MEMCPY_RETURN_TYPE void*

#include "memcpy.inc"

#undef MEMCPY_VERSION_NAME
#undef MEMCPY_SRC_QUALIFIER_BEFORE
#undef MEMCPY_SRC_QUALIFIER_AFTER
#undef MEMCPY_DST_QUALIFIER_BEFORE
#undef MEMCPY_DST_QUALIFIER_AFTER
#undef MEMCPY_RETURN_TYPE

/*-----------------------------------------------*/

#define MEMCPY_VERSION_NAME memcpy_to_volatile
#define MEMCPY_SRC_QUALIFIER_BEFORE const
#define MEMCPY_SRC_QUALIFIER_AFTER restrict
#define MEMCPY_DST_QUALIFIER_BEFORE volatile
#define MEMCPY_DST_QUALIFIER_AFTER restrict
#define MEMCPY_RETURN_TYPE volatile void*

#include "memcpy.inc"

#undef MEMCPY_VERSION_NAME
#undef MEMCPY_SRC_QUALIFIER_BEFORE
#undef MEMCPY_SRC_QUALIFIER_AFTER
#undef MEMCPY_DST_QUALIFIER_BEFORE
#undef MEMCPY_DST_QUALIFIER_AFTER
#undef MEMCPY_RETURN_TYPE

/*-----------------------------------------------*/

#define MEMCPY_VERSION_NAME memcpy_from_volatile
#define MEMCPY_SRC_QUALIFIER_BEFORE const volatile
#define MEMCPY_SRC_QUALIFIER_AFTER restrict
#define MEMCPY_DST_QUALIFIER_BEFORE
#define MEMCPY_DST_QUALIFIER_AFTER restrict
#define MEMCPY_RETURN_TYPE void*

#include "memcpy.inc"

#undef MEMCPY_VERSION_NAME
#undef MEMCPY_SRC_QUALIFIER_BEFORE
#undef MEMCPY_SRC_QUALIFIER_AFTER
#undef MEMCPY_DST_QUALIFIER_BEFORE
#undef MEMCPY_DST_QUALIFIER_AFTER
#undef MEMCPY_RETURN_TYPE
