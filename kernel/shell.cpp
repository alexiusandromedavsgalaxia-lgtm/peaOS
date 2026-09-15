#include "shell.hpp"
#include "console.hpp"
#include <stdint.h>

namespace shell {

void run() {
    console::write_line("");
    console::write_line("peaOS X90 shell");
    console::write_line("Type 'help' for commands.");
    console::write("peaOS> ");
    for (;;) asm volatile("hlt");
}

}
