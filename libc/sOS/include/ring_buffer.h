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

/*
 * {size} - must be a power of 2
 */
static inline errors_t ring_buffer_init(ring_buffer_t* ring_buffer, byte* buffer, size_t size)
{
    if ( buffer == NULL || size == 0 || (size & (size - 1)) != 0)
    {
        return ERRORS_INVALID_PARAMETERS;
    }


    *ring_buffer = (ring_buffer_t)
    {
        .size = size,
        .head = 0,
        .tail = 0,
        .buffer = buffer
    };

    return ERRORS_NONE;
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
    return ring_buffer->head >= ring_buffer->tail + ring_buffer->size;
}

static inline errors_t ring_buffer_force_write(ring_buffer_t* ring_buffer, byte* src, size_t src_size)
{
    size_t remaining_space = ring_buffer_get_remaining_space(ring_buffer);
    if ( remaining_space < src_size )
    {
        if ( src_size <= ring_buffer->size )
        {
            ring_buffer->tail += src_size - remaining_space;
        }
        else
        {
            return ERRORS_WRITE_NOT_ENOUGH_SPACE;
        }
    }

    for (size_t i = 0; i < src_size; ++i)
    {
        ring_buffer->buffer[ring_buffer->head & (ring_buffer->size - 1)] = src[i];
        ++ring_buffer->head;
    }

    return ERRORS_NONE;
}

static inline errors_t ring_buffer_try_write(ring_buffer_t* ring_buffer, byte* src, size_t src_size)
{

    if ( ring_buffer_get_remaining_space(ring_buffer) < src_size )
    {
        return ERRORS_WRITE_NOT_ENOUGH_SPACE;
    }

    for (size_t i = 0; i < src_size;
            ++i, ++ring_buffer->head)
    {
        ring_buffer->buffer[ring_buffer->head & (ring_buffer->size - 1)] = src[i];
    }

    return ERRORS_NONE;
}

static inline size_t ring_buffer_read(ring_buffer_t* ring_buffer, byte* dst, size_t dst_size)
{
    size_t i = 0;
    for (; i < dst_size && !ring_buffer_get_is_empty(ring_buffer);
            ++i, ++ring_buffer->tail)
    {
        dst[i] = ring_buffer->buffer[ring_buffer->tail & (ring_buffer->size - 1)];
    }

    return i;
}

#endif /* RING_BUFFER_H */
