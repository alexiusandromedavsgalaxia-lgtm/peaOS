#include "shell.hpp"
#include "console.hpp"
#include "memory.hpp"
#include "process.hpp"
#include "thread.hpp"
#include "activation.hpp"
#include "initial_setup.hpp"
#include "x90_features.hpp"
#include "hardware_probe.hpp"
#include "vfs.hpp"
#include "desktop.hpp"
#include "ui.hpp"
#include "app_manager.hpp"
#include "../apps/system/app_registry.hpp"
#include <stdint.h>

namespace shell {
namespace {
constexpr uint32_t kMaxCommand = 96;

static inline uint8_t keyboard_status() { uint8_t v; asm volatile("inb %1, %0" : "=a"(v) : "Nd"(0x64)); return v; }
static inline uint8_t keyboard_data() { uint8_t v; asm volatile("inb %1, %0" : "=a"(v) : "Nd"(0x60)); return v; }

char translate(uint8_t code) {
    static const char table[] = "??1234567890-=??qwertyuiop[]?asdfghjkl;'`?\\zxcvbnm,./?";
    if (code < 2 || code >= sizeof(table) - 1) return 0;
    const char c = table[code];
    return c == '?' ? 0 : c;
}

bool equal(const char* a, const char* b) { while (*a && *b && *a == *b) { ++a; ++b; } return *a == 0 && *b == 0; }
bool begins(const char* a, const char* b) { while (*b) if (*a++ != *b++) return false; return true; }

void list_apps() {
    uint32_t count = 0;
    const app_registry::AppDescriptor* apps = app_registry::all(&count);
    console::write("Installed applications: ");
    console::write_uint(count);
    console::put('\n');
    for (uint32_t i = 0; i < count; ++i) {
        console::write("  ");
        console::write(apps[i].name);
        console::write(apps[i].isolated ? " [isolated]" : "");
        console::write_line(apps[i].requires_network ? " [network]" : "");
    }
}

void launch_app(const char* name) {
    const app_registry::AppDescriptor* app = app_registry::find_by_name(name);
    if (!app) {
        console::write_line("Application not found.");
        return;
    }
    app_manager::launch(app->id);
    console::write_line(app_manager::last_status());
}

void execute(const char* command) {
    if (!command[0]) return;
    if (equal(command, "help")) {
        console::write_line("help version clear mem cpu apps open NAME");
        console::write_line("processes threads activation setup vfs windows");
        console::write_line("morph snapshot fusion ui echo TEXT");
    } else if (equal(command, "version")) console::write_line("peaOS 1 Beta 1 | X90 x86_64 | 64-bit");
    else if (equal(command, "clear")) console::clear();
    else if (equal(command, "mem")) { console::write("Heap bytes used: "); console::write_uint(memory::bytes_used()); console::put('\n'); }
    else if (equal(command, "cpu")) {
        const hardware_probe::Result r = hardware_probe::probe(0);
        console::write_line(r.cpuid_available ? "CPUID: OK" : "CPUID: unavailable");
        console::write_line(r.long_mode_available ? "Long mode: OK" : "Long mode: unknown");
        console::write_line(r.profile.cpu_supported ? "CPU baseline: supported" : "CPU baseline: unsupported");
    } else if (equal(command, "apps")) list_apps();
    else if (begins(command, "open ")) launch_app(command + 5);
    else if (equal(command, "processes")) { console::write("Processes: "); console::write_uint(process::count()); console::put('\n'); }
    else if (equal(command, "threads")) { console::write("Threads: "); console::write_uint(thread::count()); console::put('\n'); }
    else if (equal(command, "activation")) {
        const activation::State s = activation::state();
        console::write_line(s == activation::State::Unactivated ? "Activation: unactivated" : s == activation::State::Activating ? "Activation: activating" : s == activation::State::Active ? "Activation: active" : s == activation::State::OfflineGrace ? "Activation: offline grace" : s == activation::State::Revoked ? "Activation: revoked" : "Activation: failed");
    } else if (equal(command, "setup")) { console::write("Initial Setup: "); console::write_line(initial_setup::title()); console::write_line(initial_setup::status_line()); }
    else if (equal(command, "vfs")) { console::write("VFS nodes: "); console::write_uint(vfs::count()); console::put('\n'); }
    else if (equal(command, "windows")) { uint32_t count = 0; desktop::windows(&count); console::write("Windows: "); console::write_uint(count); console::put('\n'); }
    else if (equal(command, "morph")) { const x90_features::MorphState& s = x90_features::morph_state(); console::write("Morph: "); console::write(s.name); console::write(" / "); console::write_line(s.mode); }
    else if (equal(command, "snapshot")) { console::write("Snapshot ID: "); console::write_uint(x90_features::filesystem_snapshot().id); console::put('\n'); }
    else if (equal(command, "fusion")) { console::write("Fusion: "); console::write_line(x90_features::fusion_runtime().name); }
    else if (equal(command, "ui")) { console::write("UI: "); console::write(ui::style_name()); console::write(" "); console::write_uint(ui::width()); console::write("x"); console::write_uint(ui::height()); console::put('\n'); }
    else if (begins(command, "echo ")) console::write_line(command + 5);
    else { console::write("Unknown command: "); console::write_line(command); console::write_line("Type 'help' for commands."); }
}
}

void run() {
    char command[kMaxCommand]{};
    uint32_t length = 0;
    console::write_line("");
    console::write_line("peaOS X90 shell");
    console::write_line("Minimal desktop services are online.");
    console::write_line("Type 'apps' to see the default suite or 'open NAME' to launch one.");
    console::write("peaOS> ");
    for (;;) {
        if ((keyboard_status() & 1u) == 0) { asm volatile("hlt"); continue; }
        const uint8_t code = keyboard_data();
        if (code & 0x80u) continue;
        if (code == 0x1Cu) { command[length] = 0; console::put('\n'); execute(command); length = 0; command[0] = 0; console::write("peaOS> "); }
        else if (code == 0x0Eu) { if (length) { --length; command[length] = 0; console::put('\b'); } }
        else { const char c = translate(code); if (c && length + 1 < kMaxCommand) { command[length++] = c; command[length] = 0; console::put(c); } }
    }
}
}
