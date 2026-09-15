#include "console.hpp"
#include <stdint.h>

namespace {
volatile uint16_t* const VGA = reinterpret_cast<volatile uint16_t*>(0xB8000);
uint8_t row = 0;
uint8_t column = 0;
constexpr uint8_t color = 0x0F;
constexpr uint16_t SERIAL = 0x3F8;

static inline void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" :: "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void serial_init() {
    outb(SERIAL + 1, 0x00);
    outb(SERIAL + 3, 0x80);
    outb(SERIAL + 0, 0x03);
    outb(SERIAL + 1, 0x00);
    outb(SERIAL + 3, 0x03);
    outb(SERIAL + 2, 0xC7);
    outb(SERIAL + 4, 0x0B);
}

void serial_put(char c) {
    if (c == '\n') serial_put('\r');
    // A physical machine may have no working COM1.  Serial logging must
    // never prevent the VGA console or the kernel from booting.
    for (uint32_t timeout = 0; timeout < 100000u; ++timeout) {
        if (inb(SERIAL + 5) & 0x20u) {
            outb(SERIAL, static_cast<uint8_t>(c));
            return;
        }
    }
}

void newline() {
    column = 0;
    serial_put('\n');
    if (++row >= 25) {
        for (uint32_t y = 1; y < 25; ++y)
            for (uint32_t x = 0; x < 80; ++x)
                VGA[(y - 1) * 80 + x] = VGA[y * 80 + x];
        for (uint32_t x = 0; x < 80; ++x)
            VGA[24 * 80 + x] = static_cast<uint16_t>(' ') | (static_cast<uint16_t>(color) << 8);
        row = 24;
    }
}
}

namespace console {
void clear() {
    serial_init();
    for (uint32_t i = 0; i < 80 * 25; ++i)
        VGA[i] = static_cast<uint16_t>(' ') | (static_cast<uint16_t>(color) << 8);
    row = column = 0;
}

void put(char c) {
    if (c == '\n') { newline(); return; }
    if (c == '\r') { column = 0; serial_put(c); return; }
    if (c == '\b') {
        if (column) {
            --column;
            VGA[row * 80 + column] = static_cast<uint16_t>(' ') | (static_cast<uint16_t>(color) << 8);
        }
        serial_put(c);
        return;
    }
    VGA[row * 80 + column] = static_cast<uint16_t>(c) | (static_cast<uint16_t>(color) << 8);
    serial_put(c);
    if (++column >= 80) newline();
}

void write(const char* text) {
    if (!text) return;
    while (*text) put(*text++);
}

void write_line(const char* text) {
    write(text);
    put('\n');
}

void write_uint(uint64_t value) {
    char buffer[21];
    uint32_t length = 0;
    if (value == 0) { put('0'); return; }
    while (value != 0 && length < sizeof(buffer)) {
        buffer[length++] = static_cast<char>('0' + (value % 10));
        value /= 10;
    }
    while (length) put(buffer[--length]);
}
}
