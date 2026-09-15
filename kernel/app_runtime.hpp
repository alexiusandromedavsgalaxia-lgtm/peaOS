#pragma once
#include <stdint.h>
#include "../apps/system/app_registry.hpp"
#include "process.hpp"

namespace app_runtime {
enum class State : uint8_t { Empty, Starting, Running, Suspended, Stopping, Crashed };
enum class Result : uint8_t { Ok, InvalidApp, NotInstalled, NetworkRequired, UnsupportedRuntime, LimitReached, AlreadyRunning, ProcessCreateFailed };

struct Process {
    uint32_t pid;
    process::ProcessId kernel_pid;
    app_registry::AppId app;
    app_registry::Runtime runtime;
    State state;
    uint8_t priority;
    uint32_t memory_limit_kb;
    bool isolated;
    bool network;
    uint64_t entry_point;
    uint64_t ticks;
};

constexpr uint32_t kMaxProcesses = 64;
void init();
Result start(app_registry::AppId id, bool network_available);
Result stop(app_registry::AppId id);
Result suspend(app_registry::AppId id);
Result resume(app_registry::AppId id);
void tick(uint64_t ticks);
const Process* processes(uint32_t* count);
const Process* process_for(app_registry::AppId id);
uint32_t process_count();
const char* runtime_name(app_registry::Runtime runtime);
}
