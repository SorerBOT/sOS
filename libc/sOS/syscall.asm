%define SOFTWARE_INT_SYSCALL 0x80

global send_syscall

send_syscall:
    int SOFTWARE_INT_SYSCALL
    ret
