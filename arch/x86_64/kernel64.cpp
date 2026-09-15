#include <stdint.h>
#include "kernel/console.hpp"
#include "kernel/memory.hpp"
#include "kernel/x90_features.hpp"

extern "C" void kernel_main64(uint64_t magic, uint64_t multiboot_info) {
    console::clear();
    console::write_line("peaOS 1 Beta 1");
    console::write_line("X90 | x86_64 | 64-bit");
    console::write_line("========================================");
    if (magic == 0x2BADB002ULL) console::write_line("Multiboot: OK");
    else console::write_line("Multiboot: invalid magic");
    console::write_line("CPU mode: x86_64 long mode");
    console::write_line("C++ kernel: online");
    memory::init();
    x90_features::init();
    console::write_line("X90 Morph: ready");
    console::write_line("X90 filesystem snapshots: ready");
    console::write_line("X90 Fusion: ready");
    console::write_line("Bootstrap heap: online");
    (void)multiboot_info;
    for (;;) asm volatile("hlt");
}
