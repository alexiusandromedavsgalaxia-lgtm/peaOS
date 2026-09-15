#include "shell.hpp"
#include "console.hpp"
#include <stdint.h>

namespace shell {
void run() {
    console::write_line("peaOS shell v0.1");
    console::write_line("Native command runtime is being brought online.");
    console::write_line("Commands: help, version, mem, clear");
    console::write("peaOS> ");
    for (;;) asm volatile("hlt");
}
}
