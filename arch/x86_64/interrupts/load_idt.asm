global load_idt

load_idt:
    push rbp
    mov rbp, rsp

    lidt rdi
    sti

    pop rsp
