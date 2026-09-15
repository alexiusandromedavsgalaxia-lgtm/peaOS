#include "thread.hpp"

namespace thread {
namespace {
Thread g_threads[kMaxThreads]{};
uint32_t g_count = 0;
uint32_t g_current = 0;
ThreadId g_next_id = 1;
}

void init() {
    for (uint32_t i = 0; i < kMaxThreads; ++i) g_threads[i] = {};
    g_count = 0;
    g_current = 0;
    g_next_id = 1;
}

ThreadId create(uint64_t entry_point, uint64_t stack_pointer) {
    for (uint32_t i = 0; i < kMaxThreads; ++i) {
        if (!g_threads[i].runnable) {
            g_threads[i] = {g_next_id++, stack_pointer, entry_point, true};
            ++g_count;
            return g_threads[i].id;
        }
    }
    return 0;
}

bool stop(ThreadId id) {
    for (uint32_t i = 0; i < kMaxThreads; ++i) {
        if (g_threads[i].runnable && g_threads[i].id == id) {
            g_threads[i].runnable = false;
            if (g_count) --g_count;
            if (g_current == i) g_current = 0;
            return true;
        }
    }
    return false;
}

const Thread* current() {
    if (g_count == 0) return nullptr;
    return &g_threads[g_current];
}

void schedule_tick() {
    if (g_count == 0) return;
    for (uint32_t offset = 1; offset <= kMaxThreads; ++offset) {
        const uint32_t index = (g_current + offset) % kMaxThreads;
        if (g_threads[index].runnable) {
            g_current = index;
            return;
        }
    }
}

uint32_t count() { return g_count; }

}
