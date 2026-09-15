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
    cld
    mov esp, stack_top
    mov [multiboot_magic], eax
    mov [multiboot_info_ptr], ebx

    lgdt [gdt64_descriptor]

    ; Enable PAE.
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Enable long mode through EFER.LME.
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Install the identity map for the first 2 MiB.
    mov eax, page_table_l4
    mov cr3, eax

    ; Enable paging while protected mode is already active.
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; Far jump reloads CS and enters 64-bit mode.
    jmp 0x08:long_mode_entry

BITS 64
long_mode_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    xor eax, eax
    mov fs, ax
    mov gs, ax

    ; Keep SysV x86_64 stack alignment at the C++ boundary.
    and rsp, -16
    sub rsp, 8
    mov rdi, [multiboot_magic]
    mov rsi, [multiboot_info_ptr]
    call kernel_main64

.hang:
    cli
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
