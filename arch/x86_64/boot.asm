BITS 32
section .multiboot
align 4
mb_header:
    dd 0x1BADB002
    dd 0
    dd -(0x1BADB002)

section .text
extern kernel_main64
global _start
_start:
    cli
    mov esp, stack_top
    mov [multiboot_magic], eax
    mov [multiboot_info_ptr], ebx

    lgdt [gdt64_descriptor]
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov eax, page_table_l4
    mov cr3, eax
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    jmp 0x08:long_mode_entry

BITS 64
long_mode_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov rdi, [multiboot_magic]
    mov rsi, [multiboot_info_ptr]
    call kernel_main64
.hang:
    hlt
    jmp .hang

section .data
align 8
multiboot_magic: dq 0
multiboot_info_ptr: dq 0

gdt64:
    dq 0
    dq 0x0020980000000000
    dq 0x0000920000000000
gdt64_descriptor:
    dw gdt64_descriptor - gdt64 - 1
    dq gdt64

section .bss
align 4096
page_table_l4:
    dq page_table_pdpt + 0x003
    times 511 dq 0
page_table_pdpt:
    dq page_table_pd + 0x003
    times 511 dq 0
page_table_pd:
    dq 0x0000000000000083
    times 511 dq 0

align 16
stack_bottom:
    resb 16384
stack_top:
