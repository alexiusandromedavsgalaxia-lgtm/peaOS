#include "console.hpp"
#include <stdint.h>

namespace {
volatile uint16_t* const VGA = reinterpret_cast<volatile uint16_t*>(0xB8000);
uint8_t row = 0;
uint8_t column = 0;
constexpr uint8_t color = 0x0F;

void newline() {
    column = 0;
    if (++row >= 25) {
        for (uint32_t y = 1; y < 25; ++y)
            for (uint32_t x = 0; x < 80; ++x)
                VGA[(y - 1) * 80 + x] = VGA[y * 80 + x];
        for (uint32_t x = 0; x < 80; ++x) VGA[24 * 80 + x] = static_cast<uint16_t>(' ') | (static_cast<uint16_t>(color) << 8);
        row = 24;
    }
}
}

namespace console {
void clear() {
    for (uint32_t i = 0; i < 80 * 25; ++i) VGA[i] = static_cast<uint16_t>(' ') | (static_cast<uint16_t>(color) << 8);
    row = column = 0;
}
void put(char c) {
    if (c == '\n') { newline(); return; }
    if (c == '\r') { column = 0; return; }
    if (c == '\b') {
        if (column) { --column; VGA[row * 80 + column] = static_cast<uint16_t>(' ') | (static_cast<uint16_t>(color) << 8); }
        return;
    }
    VGA[row * 80 + column] = static_cast<uint16_t>(c) | (static_cast<uint16_t>(color) << 8);
    if (++column >= 80) newline();
}
void write(const char* text) { while (*text) put(*text++); }
void write_line(const char* text) { write(text); put('\n'); }
}
