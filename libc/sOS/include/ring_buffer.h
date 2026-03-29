#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <types.h>


typedef struct
{
    size_t size;
    size_t head;
    size_t tail;
    byte* buffer;
} ring_buffer_t;

static inline size_t ring_buffer_get_content_size(ring_buffer_t* ring_buffer);
static inline size_t ring_buffer_get_remaining_space(ring_buffer_t* ring_buffer);
static inline bool ring_buffer_get_is_empty(ring_buffer_t* ring_buffer);
static inline bool ring_buffer_get_is_full(ring_buffer_t* ring_buffer);

static inline size_t ring_buffer_get_content_size(ring_buffer_t* ring_buffer)
{
    return ring_buffer->head - ring_buffer->tail;
}

static inline size_t ring_buffer_get_remaining_space(ring_buffer_t* ring_buffer)
{
    return (ring_buffer->size) - (ring_buffer->head - ring_buffer->tail);
}

static inline bool ring_buffer_get_is_empty(ring_buffer_t* ring_buffer)
{
    return ring_buffer->head == ring_buffer->tail;
}

static inline bool ring_buffer_get_is_full(ring_buffer_t* ring_buffer)
{
    /*
     * unsigned int subtraction supports wrapping
     * its important that the variable which wraps
     * first is the one subtracted from
     */
    return (ring_buffer->head - ring_buffer->tail) >= ring_buffer->size;
}

/*
 * {size} - must be a power of 2
 */
errors_t ring_buffer_init(ring_buffer_t* ring_buffer, byte* buffer, size_t size);
errors_t ring_buffer_force_write(ring_buffer_t* ring_buffer, byte* src, size_t src_size);
errors_t ring_buffer_try_write(ring_buffer_t* ring_buffer, byte* src, size_t src_size);
errors_t ring_buffer_read(ring_buffer_t* ring_buffer, byte* dst, size_t dst_size);

#endif /* RING_BUFFER_H */
