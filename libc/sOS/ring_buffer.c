#include <ring_buffer.h>

errors_t ring_buffer_init(ring_buffer_t* ring_buffer, byte* buffer, size_t size)
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

errors_t ring_buffer_force_write(ring_buffer_t* ring_buffer, byte* src, size_t src_size)
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

errors_t ring_buffer_try_write(ring_buffer_t* ring_buffer, byte* src, size_t src_size)
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

errors_t ring_buffer_read(ring_buffer_t* ring_buffer, byte* dst, size_t dst_size)
{
    if ( dst_size > ring_buffer_get_content_size(ring_buffer) )
    {
        return ERRORS_NOT_ENOUGH_DATA;
    }

    size_t i = 0;
    for (; i < dst_size && !ring_buffer_get_is_empty(ring_buffer);
            ++i, ++ring_buffer->tail)
    {
        dst[i] = ring_buffer->buffer[ring_buffer->tail & (ring_buffer->size - 1)];
    }

    return ERRORS_NONE;
}
