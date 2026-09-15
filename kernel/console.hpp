#pragma once
#include <stdint.h>

namespace console {
void clear();
void write(const char* text);
void write_line(const char* text);
void put(char c);
}
