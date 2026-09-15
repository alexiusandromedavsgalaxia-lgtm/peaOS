#include "memory.hpp"

namespace {
alignas(16) static uint8_t heap[1024 * 1024];
uint32_t cursor = 0;
}

namespace memory {
void init() {
    cursor = 0;
}

void* allocate(uint32_t bytes) {
    if (bytes == 0) return nullptr;

    const uint32_t aligned = (cursor + 15u) & ~15u;
    if (aligned > sizeof(heap) || bytes > sizeof(heap) - aligned) return nullptr;

    cursor = aligned + bytes;
    return &heap[aligned];
}

uint32_t bytes_used() {
    return cursor;
}
}
