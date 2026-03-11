#ifndef PROCESS_TYPES_H
#define PROCESS_TYPES_H

#include <types.h>

typedef size_t process_id_t;
typedef void(*process_routine_t)(void*);

typedef struct
{
    process_id_t pid;
    const void* rsp;
} process_context_t;

#endif /* PROCESS_TYPES_H */
