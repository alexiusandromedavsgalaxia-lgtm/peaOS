#include "shell.hpp"
#include "console.hpp"
#include <stdint.h>

namespace shell {
namespace {
constexpr uint32_t kMaxCommand = 96;

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

char translate(uint8_t code) {
    static const char table[] = "??1234567890-=??qwertyuiop[]?asdfghjkl;'`?\\zxcvbnm,./?";
    if (code < 2 || code >= sizeof(table) - 1) return 0;
    const char c = table[code];
    return c == '?' ? 0 : c;
}

bool equal(const char* a, const char* b) {
    while (*a && *b && *a == *b) { ++a; ++b; }
    return *a == 0 && *b == 0;
}

bool begins(const char* a, const char* b) {
    while (*b) if (*a++ != *b++) return false;
    return true;
}

void execute(const char* command) {
    if (!command[0]) return;
    if (equal(command, "help")) {
        console::write_line("help  version  clear  mem  cpu");
        console::write_line("processes  threads  activation  setup");
        console::write_line("morph  snapshot  fusion  echo TEXT");
    } else if (equal(command, "version")) {
        console::write_line("peaOS 1 Beta 1 | X90 x86_64 | 64-bit");
    } else if (equal(command, "clear")) {
        console::clear();
    } else if (begins(command, "echo ")) {
        console::write_line(command + 5);
    } else {
        console::write("Unknown command: ");
        console::write_line(command);
        console::write_line("Type 'help' for commands.");
    }
}
}

void run() {
    char command[kMaxCommand]{};
    uint32_t length = 0;
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
        if (code & 0x80u) continue;

        if (code == 0x1Cu) {
            command[length] = 0;
            console::put('\n');
            execute(command);
            length = 0;
            command[0] = 0;
            console::write("peaOS> ");
        } else if (code == 0x0Eu) {
            if (length) {
                --length;
                command[length] = 0;
                console::put('\b');
            }
        } else {
            const char c = translate(code);
            if (c && length + 1 < kMaxCommand) {
                command[length++] = c;
                command[length] = 0;
                console::put(c);
            }
        }
    }
}

}
