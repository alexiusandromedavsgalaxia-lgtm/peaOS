BITS 32
section .multiboot
align 4
mb_header:
    dd 0x1BADB002
    dd 0
    dd -(0x1BADB002)

section .text
extern kernel_main
global _start
_start:
    cli
    mov esp, stack_top
    push ebx
    push eax
    call kernel_main
.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:
