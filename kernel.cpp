#include <stdint.h>
#include "kernel/console.hpp"
#include "kernel/memory.hpp"
#include "kernel/process.hpp"
#include "kernel/thread.hpp"
#include "kernel/activation.hpp"
#include "kernel/initial_setup.hpp"
#include "kernel/x90_features.hpp"
#include "kernel/vfs.hpp"
#include "kernel/desktop.hpp"
#include "kernel/ui.hpp"
#include "kernel/app_manager.hpp"
#include "kernel/app_runtime.hpp"
#include "kernel/window_manager.hpp"
#include "kernel/shell.hpp"

extern "C" void kernel_main(uint32_t magic, uint32_t multiboot_info) {
    console::clear();
    console::write_line("peaOS 0.2");
    console::write_line("----------------------------------------");
    if (magic == 0x2BADB002) console::write_line("Multiboot: OK");
    else console::write_line("Multiboot: invalid magic");
    console::write_line("C++ kernel: online");

    memory::init();
    process::init();
    thread::init();
    activation::init();
    initial_setup::init();
    x90_features::init();
    vfs::init();
    desktop::init(1024, 768);
    ui::init(1024, 768);
    app_runtime::init();
    app_manager::init();
    window_manager::init(1024, 768);

    console::write_line("Bootstrap heap: 1 MiB online");
    console::write_line("Initial Setup: privileged system service online");
    console::write_line(activation::state() == activation::State::Unactivated
        ? "Activation state: unactivated" : "Activation state: active");
    console::write_line("X90 Morph: ready");
    console::write_line("X90 filesystem snapshots: ready");
    console::write_line("X90 Fusion: ready");
    console::write_line("Advanced window manager: ready");
    console::write_line("Application runtime: ready");
    console::write_line("Application reader: ready");
    console::write_line("Application manager: ready");

    (void)multiboot_info;
    shell::run();
    for (;;) asm volatile("hlt");
}
