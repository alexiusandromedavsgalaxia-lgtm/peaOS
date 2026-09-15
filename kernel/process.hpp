#pragma once
#include <stdint.h>

namespace process {
using ProcessId = uint64_t;
using EntryPoint = void (*)();

struct Process {
    ProcessId id;
    uint64_t address_space;
    uint64_t entry_point;
    EntryPoint entry;
    uint8_t privilege;
    bool alive;
    bool runnable;
    uint64_t ticks;
};

constexpr uint32_t kMaxProcesses = 64;
void init();
ProcessId create(uint64_t entry_point, uint8_t privilege);
ProcessId create(EntryPoint entry, uint8_t privilege);
bool terminate(ProcessId id);
const Process* current();
uint32_t count();
void schedule_tick();
void run_current();
}
