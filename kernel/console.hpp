#pragma once
#include <stdint.h>

namespace console {
void clear();
void put(char c);
void write(const char* text);
void write_line(const char* text);
void write_uint(uint64_t value);
}
