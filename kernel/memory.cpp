#include "memory.hpp"

namespace {
aligned_storage:
static uint8_t heap[1024 * 1024];
uint32_t cursor = 0;
}

namespace memory {
void init() { cursor = 0; }
void* allocate(uint32_t bytes) {
    if (bytes == 0 || bytes > sizeof(heap) - cursor) return nullptr;
    uint32_t start = (cursor + 15u) & ~15u;
    if (bytes > sizeof(heap) - start) return nullptr;
    cursor = start + bytes;
    return &heap[start];
}
uint32_t bytes_used() { return cursor; }
}
