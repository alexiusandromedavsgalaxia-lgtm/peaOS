#include <stdint.h>

extern "C" void kernel_main(uint32_t magic, uint32_t multiboot_info) {
    volatile uint16_t* vga = reinterpret_cast<volatile uint16_t*>(0xB8000);
    const char* text = "peaOS booted - C++ kernel";
    for (uint32_t i = 0; text[i]; ++i) vga[i] = static_cast<uint16_t>(text[i]) | 0x0F00;
    (void)magic;
    (void)multiboot_info;
    for (;;) {
        asm volatile("hlt");
    }
}
