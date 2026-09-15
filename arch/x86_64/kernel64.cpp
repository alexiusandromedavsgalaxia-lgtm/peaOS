#include <stdint.h>
#include "certificate_registry.hpp"
#include "kernel/console.hpp"
#include "kernel/memory.hpp"
#include "kernel/version.hpp"
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
#include "kernel/app_ui.hpp"
#include "kernel/app_manager.hpp"
#include "kernel/app_runtime.hpp"
#include "kernel/drivers/driver.hpp"
#include "kernel/drivers/pci.hpp"
#include "kernel/drivers/network.hpp"
#include "kernel/drivers/bluetooth.hpp"
#include "kernel/drivers/usb.hpp"

extern "C" void kernel_main64(uint64_t magic, uint64_t multiboot_info) {
    console::clear();
    console::write_line("peaOS 1 Beta 1");
    console::write("Kernel version: ");
    console::write_line(kernel_version::kFullVersion);
    console::write_line("X90 | x86_64 | 64-bit");
    console::write_line("========================================");

    if (magic == 0x2BADB002ULL) console::write_line("Multiboot: OK");
    else console::write_line("Multiboot: invalid magic");

    console::write_line("CPU mode: x86_64 long mode");
    console::write_line("C++ kernel: online");

    memory::init();
    drivers::init();
    drivers::pci::init();
    drivers::network::init();
    drivers::bluetooth::init();
    drivers::usb::init();
    const uint32_t pci_count = drivers::pci::enumerate();
    const uint32_t net_count = drivers::network::probe_pci();

    x90_features::init();
    activation::init();
    initial_setup::init();
    permissions::init();
    process::init();
    thread::init();
    vfs::init();
    desktop::init(1024, 768);
    ui::init(1024, 768);
    app_ui::init(1024, 768);
    app_runtime::init();
    app_manager::init();

    const hardware_probe::Result hardware = hardware_probe::probe(magic, multiboot_info);
    console::write_line(hardware.cpuid_available ? "CPUID: OK" : "CPUID: unavailable");
    console::write_line(hardware.long_mode_available ? "Long mode capability: OK" : "Long mode capability: unknown");
    console::write_line(hardware.profile.cpu_supported ? "CPU baseline: supported" : "CPU baseline: unsupported");
    console::write_line(hardware.profile.ram_bytes >= hardware::kMinimumRamBytes ? "RAM minimum: OK" : "RAM minimum: FAIL");
    console::write("PCI devices: "); console::write_uint(pci_count); console::put('\n');
    console::write("Supported Ethernet adapters (probe only): "); console::write_uint(net_count); console::put('\n');
    console::write_line("Bluetooth adapter registry: initialized (no HCI transport yet)");
    console::write_line("USB device model: initialized (no controller transport yet)");
    console::write_line("Storage: VFS bootstrap online");
    console::write_line("Desktop: window manager state online");
    console::write("Interface: ");
    console::write_line(ui::style_name());
    console::write_line("Application surfaces: per-app state and event routing online");
    console::write_line("Firmware validation: bootstrap profile");

    console::write_line("Certificate registry: synchronized build snapshot");
    console::write("Certificate issuer: "); console::write_line(certificate_registry::kIssuer);
    console::write("Certificate version: "); console::write_uint(certificate_registry::kCertificateVersion); console::put('\n');
    console::write("Official certificates in registry: "); console::write_uint(certificate_registry::kCertificateCount); console::put('\n');
    console::write("Registry SHA-256: "); console::write_line(certificate_registry::kRegistrySha256);

    console::write_line("Initial Setup: state machine initialized");
    console::write_line("First boot: activation is part of Initial Setup");
    console::write_line("Activation state: unactivated (fail-closed until online verification)");
    console::write_line("Permissions: default-deny policy online");
    console::write_line("Process manager: kernel-backed scheduling metadata online");
    console::write_line("Thread manager: state registry initialized");
    console::write_line("X90 syscall ABI: definitions loaded (dispatcher isolation pending)");
    console::write_line("X90 Morph: state manager initialized");
    console::write_line("X90 filesystem snapshots: metadata manager initialized");
    console::write_line("X90 Fusion: runtime registry initialized");
    console::write_line("Bundled apps: default suite registered");
    console::write_line("Application manager: online");
    console::write_line("Bootstrap heap: online");

    ui::render_home();
    (void)syscall_api::kAbiVersion;
    shell::run();
}
