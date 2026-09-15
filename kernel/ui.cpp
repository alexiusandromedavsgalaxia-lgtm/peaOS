#include "ui.hpp"
#include "console.hpp"

namespace ui {
namespace {
Theme g_theme{0x101114, 0x181A20, 0x22252D, 0x8B7CFF, 0xF4F5F7, 0xA5A9B2, 14, 12};
uint32_t g_width = 1024;
uint32_t g_height = 768;
}

void init(uint32_t width, uint32_t height) {
    g_width = width ? width : 1024;
    g_height = height ? height : 768;
}

const Theme& theme() { return g_theme; }
uint32_t width() { return g_width; }
uint32_t height() { return g_height; }
const char* style_name() { return "X90 Minimal"; }

void render_home() {
    console::write_line("");
    console::write_line("+----------------------------------------------------------------+");
    console::write_line("| peaOS                                      X90 Minimal         |");
    console::write_line("|                                                                |");
    console::write_line("|  Favorites      Files   Browser   Notes   Settings             |");
    console::write_line("|                                                                |");
    console::write_line("|  System         Terminal   Software Center   Task Manager      |");
    console::write_line("|  Media          Photos     Image Viewer      Media Player      |");
    console::write_line("|  Productivity   Text Editor  Calendar        Clock             |");
    console::write_line("|  X90             Fusion     TimeMachine      Morph             |");
    console::write_line("|                                                                |");
    console::write_line("|  peaOS 1 Beta 1                         x86_64   online       |");
    console::write_line("+----------------------------------------------------------------+");
}

}
