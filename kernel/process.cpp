#include "process.hpp"

namespace process {
namespace {
Process g_processes[kMaxProcesses]{};
uint32_t g_count = 0;
uint32_t g_current = 0;
ProcessId g_next_id = 1;
}

void init() {
    for (uint32_t i = 0; i < kMaxProcesses; ++i) g_processes[i] = {};
    g_count = 0;
    g_current = 0;
    g_next_id = 1;
}

ProcessId create(uint64_t entry_point, uint8_t privilege) {
    return create(reinterpret_cast<EntryPoint>(entry_point), privilege);
}

ProcessId create(EntryPoint entry, uint8_t privilege) {
    for (uint32_t i = 0; i < kMaxProcesses; ++i) {
        if (!g_processes[i].alive) {
            g_processes[i] = {g_next_id++, 0, reinterpret_cast<uint64_t>(entry), entry, privilege, true, true, 0};
            ++g_count;
            if (g_next_id == 0) g_next_id = 1;
            return g_processes[i].id;
        }
    }
    return 0;
}

bool terminate(ProcessId id) {
    for (uint32_t i = 0; i < kMaxProcesses; ++i) {
        if (g_processes[i].alive && g_processes[i].id == id) {
            g_processes[i].alive = false;
            g_processes[i].runnable = false;
            if (g_count) --g_count;
            if (g_current == i) g_current = 0;
            return true;
        }
    }
    return false;
}

const Process* current() {
    if (g_count == 0) return nullptr;
    if (!g_processes[g_current].alive) return nullptr;
    return &g_processes[g_current];
}

uint32_t count() { return g_count; }

void schedule_tick() {
    if (g_count == 0) return;
    for (uint32_t offset = 1; offset <= kMaxProcesses; ++offset) {
        const uint32_t index = (g_current + offset) % kMaxProcesses;
        if (g_processes[index].alive && g_processes[index].runnable) {
            g_current = index;
            ++g_processes[index].ticks;
            return;
        }
    }
}

}
