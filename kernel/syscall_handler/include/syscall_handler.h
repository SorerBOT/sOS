#ifndef SYSCALL_HANDLER_H
#define SYSCALL_HANDLER_H

#include <types.h>

void syscall_handler_handle(const void* rsp, void* syscall_args, qword syscall_number);


#endif /* SYSCALL_HANDLER_H */
