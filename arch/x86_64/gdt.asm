global gdt_load

gdt_load:
    push rbp
    mov rbp, rsp

    lgdt [rdi]

    pop rbp
    ret
