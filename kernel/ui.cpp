#include "ui.hpp"

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

}
