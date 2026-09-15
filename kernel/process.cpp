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
    for (uint32_t i = 0; i < kMaxProcesses; ++i) {
        if (!g_processes[i].alive) {
            g_processes[i] = {g_next_id++, 0, entry_point, privilege, true};
            ++g_count;
            return g_processes[i].id;
        }
    }
    return 0;
}

bool terminate(ProcessId id) {
    for (uint32_t i = 0; i < kMaxProcesses; ++i) {
        if (g_processes[i].alive && g_processes[i].id == id) {
            g_processes[i].alive = false;
            if (g_count) --g_count;
            if (g_current == i) g_current = 0;
            return true;
        }
    }
    return false;
}

const Process* current() {
    if (g_count == 0) return nullptr;
    return &g_processes[g_current];
}

uint32_t count() { return g_count; }

void schedule_tick() {
    if (g_count == 0) return;
    for (uint32_t offset = 1; offset <= kMaxProcesses; ++offset) {
        const uint32_t index = (g_current + offset) % kMaxProcesses;
        if (g_processes[index].alive) {
            g_current = index;
            return;
        }
    }
}

}
