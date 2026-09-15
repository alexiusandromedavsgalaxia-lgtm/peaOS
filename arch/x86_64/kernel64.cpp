#include <stdint.h>
#include "kernel/console.hpp"
#include "kernel/memory.hpp"

extern "C" void kernel_main64(uint64_t magic, uint64_t multiboot_info) {
    console::clear();
    console::write_line("peaOS 64-bit kernel");
    console::write_line("========================================");
    if (magic == 0x2BADB002ULL) console::write_line("Multiboot: OK");
    else console::write_line("Multiboot: invalid magic");
    console::write_line("CPU mode: x86_64 long mode");
    console::write_line("C++ kernel: online");
    memory::init();
    console::write_line("Bootstrap heap: online");
    (void)multiboot_info;
    for (;;) asm volatile("hlt");
}
