#pragma once
#include <stdint.h>

namespace memory {
void init();
void* allocate(uint32_t bytes);
uint32_t bytes_used();
}
