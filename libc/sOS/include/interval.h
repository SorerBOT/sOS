#ifndef INTERVAL_H
#define INTERVAL_H

#include <types.h>

typedef struct
{
    size_t start;
    size_t end;
} interval_t;

static inline bool intervals_closed_is_intersecting(interval_t first, interval_t second)
{
    return first.start <= second.end && second.start <= first.end;
}
static inline bool intervals_open_is_intersecting(interval_t first, interval_t second)
{
    return first.start < second.end && second.start < first.end;
}

#endif /* INTERVAL_H */
