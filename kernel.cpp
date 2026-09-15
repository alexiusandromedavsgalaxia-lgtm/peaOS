#include <stdint.h>
#include "kernel/console.hpp"
#include "kernel/memory.hpp"
#include "kernel/shell.hpp"

extern "C" void kernel_main(uint32_t magic, uint32_t multiboot_info) {
    console::clear();
    console::write_line("peaOS 0.2");
    console::write_line("----------------------------------------");
    if (magic == 0x2BADB002) console::write_line("Multiboot: OK");
    else console::write_line("Multiboot: invalid magic");
    console::write_line("C++ kernel: online");
    memory::init();
    console::write_line("Bootstrap heap: 1 MiB online");
    (void)multiboot_info;
    shell::run();
    for (;;) asm volatile("hlt");
}
