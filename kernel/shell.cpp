#include "shell.hpp"
#include "console.hpp"
#include "memory.hpp"
#include "process.hpp"
#include "thread.hpp"
#include "activation.hpp"
#include "initial_setup.hpp"
#include "x90_features.hpp"
#include "hardware_probe.hpp"
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
    } else if (equal(command, "mem")) {
        console::write("Heap bytes used: ");
        console::write_uint(memory::bytes_used());
        console::put('\n');
    } else if (equal(command, "cpu")) {
        const hardware_probe::Result r = hardware_probe::probe(0);
        console::write_line(r.cpuid_available ? "CPUID: OK" : "CPUID: unavailable");
        console::write_line(r.long_mode_available ? "Long mode: OK" : "Long mode: unknown");
        console::write_line(r.profile.cpu_supported ? "CPU baseline: supported" : "CPU baseline: unsupported");
    } else if (equal(command, "processes")) {
        console::write("Processes: ");
        console::write_uint(process::count());
        console::put('\n');
    } else if (equal(command, "threads")) {
        console::write("Threads: ");
        console::write_uint(thread::count());
        console::put('\n');
    } else if (equal(command, "activation")) {
        const activation::State state = activation::state();
        if (state == activation::State::Unactivated) console::write_line("Activation: unactivated");
        else if (state == activation::State::Activating) console::write_line("Activation: activating");
        else if (state == activation::State::Active) console::write_line("Activation: active");
        else if (state == activation::State::OfflineGrace) console::write_line("Activation: offline grace");
        else if (state == activation::State::Revoked) console::write_line("Activation: revoked");
        else console::write_line("Activation: failed");
    } else if (equal(command, "setup")) {
        console::write("Initial Setup: ");
        console::write_line(initial_setup::title());
        console::write_line(initial_setup::status_line());
    } else if (equal(command, "morph")) {
        const x90_features::MorphState& state = x90_features::morph_state();
        console::write("Morph: ");
        console::write(state.name);
        console::write(" / ");
        console::write_line(state.mode);
    } else if (equal(command, "snapshot")) {
        console::write("Snapshot ID: ");
        console::write_uint(x90_features::filesystem_snapshot().id);
        console::put('\n');
    } else if (equal(command, "fusion")) {
        console::write("Fusion: ");
        console::write_line(x90_features::fusion_runtime().name);
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
