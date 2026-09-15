#include "shell.hpp"
#include "console.hpp"
#include <stdint.h>

namespace shell {
namespace {
static inline uint8_t keyboard_status() {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(0x64));
    return value;
}

static inline uint8_t keyboard_data() {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(0x60));
    return value;
}
}

void run() {
    console::write_line("");
    console::write_line("peaOS X90 shell");
    console::write_line("Type 'help' for commands.");
    console::write("peaOS> ");

    for (;;) {
        if ((keyboard_status() & 1u) == 0) {
            asm volatile("hlt");
            continue;
        }
        const uint8_t code = keyboard_data();
        if ((code & 0x80u) != 0) continue;
        if (code == 0x1Cu) console::put('\n');
    }
}

}
