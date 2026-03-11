%define SOFTWARE_INT_SYSCALL 0x80

global syscall_dispatcher_dispatch

syscall_dispatcher_dispatch:
    int SOFTWARE_INT_SYSCALL
    ret
