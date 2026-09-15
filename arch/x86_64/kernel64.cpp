#include <stdint.h>
#include "kernel/console.hpp"
#include "kernel/memory.hpp"
#include "kernel/x90_features.hpp"
#include "kernel/initial_setup.hpp"
#include "kernel/hardware_probe.hpp"
#include "kernel/activation.hpp"
#include "kernel/permissions.hpp"
#include "kernel/process.hpp"
#include "kernel/thread.hpp"
#include "kernel/syscall.hpp"
#include "kernel/shell.hpp"
#include "kernel/vfs.hpp"
#include "kernel/desktop.hpp"
#include "kernel/ui.hpp"
#include "kernel/app_manager.hpp"

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
    activation::init();
    initial_setup::init();
    permissions::init();
    process::init();
    thread::init();
    vfs::init();
    desktop::init(1024, 768);
    ui::init(1024, 768);
    app_manager::init();

    const hardware_probe::Result hardware = hardware_probe::probe(magic, multiboot_info);
    console::write_line(hardware.cpuid_available ? "CPUID: OK" : "CPUID: unavailable");
    console::write_line(hardware.long_mode_available ? "Long mode capability: OK" : "Long mode capability: unknown");
    console::write_line(hardware.profile.cpu_supported ? "CPU baseline: supported" : "CPU baseline: unsupported");
    console::write_line(hardware.profile.ram_bytes >= hardware::kMinimumRamBytes ? "RAM minimum: OK" : "RAM minimum: FAIL");
    console::write_line("Storage: VFS bootstrap online");
    console::write_line("Desktop: window manager state online");
    console::write("Interface: ");
    console::write_line(ui::style_name());
    console::write_line("Firmware validation: bootstrap profile");

    console::write_line("Initial Setup: privileged system service online");
    console::write_line("First boot: activation is part of Initial Setup");
    console::write_line("Activation state: unactivated (fail-closed until online verification)");
    console::write_line("Permissions: default-deny policy online");
    console::write_line("Process manager: online");
    console::write_line("Thread scheduler: online");
    console::write_line("X90 syscall ABI: online");
    console::write_line("X90 Morph: ready");
    console::write_line("X90 filesystem snapshots: ready");
    console::write_line("X90 Fusion: ready");
    console::write_line("Bundled apps: complete default suite registered");
    console::write_line("Application manager: online");
    console::write_line("Bootstrap heap: online");

    ui::render_home();
    (void)syscall_api::kAbiVersion;
    // Interrupts remain disabled until an IDT/PIC path exists.
    shell::run();
}
