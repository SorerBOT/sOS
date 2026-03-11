#ifndef SYSCALL_TYPES_H
#define SYSCALL_TYPES_H

typedef enum
{
    SYSCALL_LAUNCH_PROCESS,
    SYSCALL_COUNT
} syscall_type_t;

typedef void(*syscall_handler_t)(void*);

#endif /* SYSCALL_TYPES */
