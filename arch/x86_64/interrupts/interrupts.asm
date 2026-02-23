global interrupts_enable

interrupts_enable:
    push rbp
    mov rbp, rsp

    sti ; ENABLING INTERRUPTS

    pop rbp
    ret
