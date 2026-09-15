#pragma once
#include <stdint.h>

namespace thread {

using ThreadId = uint64_t;

struct Thread {
    ThreadId id;
    uint64_t stack_pointer;
    uint64_t instruction_pointer;
    bool runnable;
};

constexpr uint32_t kMaxThreads = 128;

void init();
ThreadId create(uint64_t entry_point, uint64_t stack_pointer);
bool stop(ThreadId id);
const Thread* current();
void schedule_tick();
uint32_t count();

}
