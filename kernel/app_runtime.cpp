#include "app_runtime.hpp"

namespace app_runtime {
namespace {
Process g_processes[kMaxProcesses]{};
uint32_t g_count = 0;
uint32_t g_next_pid = 1;

Process* find(app_registry::AppId id) {
    for (uint32_t i = 0; i < g_count; ++i)
        if (g_processes[i].state != State::Empty && g_processes[i].app == id) return &g_processes[i];
    return nullptr;
}
}

void init() {
    for (auto& p : g_processes) p = {};
    g_count = 0;
    g_next_pid = 1;
}

Result start(app_registry::AppId id, bool network_available) {
    const auto* app = app_registry::find(id);
    if (!app || !app->bundled) return Result::NotInstalled;
    if (find(id)) return Result::AlreadyRunning;
    if (app->requires_network && !network_available) return Result::NetworkRequired;
    if (app->runtime != app_registry::Runtime::Native &&
        app->runtime != app_registry::Runtime::X90Fusion &&
        app->runtime != app_registry::Runtime::WebSandbox) return Result::UnsupportedRuntime;
    if (g_count >= kMaxProcesses) return Result::LimitReached;

    Process& p = g_processes[g_count];
    p = {g_next_pid++, id, app->runtime, State::Starting,
         static_cast<uint8_t>(app->isolated ? 180 : 128),
         app->isolated ? 65536u : 32768u,
         app->isolated, app->requires_network, 0};
    if (g_next_pid == 0) g_next_pid = 1;
    p.state = State::Running;
    ++g_count;
    return Result::Ok;
}

Result stop(app_registry::AppId id) {
    Process* p = find(id);
    if (!p) return Result::NotInstalled;
    const uint32_t index = static_cast<uint32_t>(p - g_processes);
    const uint32_t last = g_count - 1;
    if (index != last) g_processes[index] = g_processes[last];
    g_processes[last] = {};
    --g_count;
    return Result::Ok;
}

Result suspend(app_registry::AppId id) {
    Process* p = find(id);
    if (!p) return Result::NotInstalled;
    if (p->state != State::Running) return Result::InvalidApp;
    p->state = State::Suspended;
    return Result::Ok;
}

Result resume(app_registry::AppId id) {
    Process* p = find(id);
    if (!p) return Result::NotInstalled;
    if (p->state != State::Suspended) return Result::InvalidApp;
    p->state = State::Running;
    return Result::Ok;
}

void tick(uint64_t ticks) {
    for (uint32_t i = 0; i < g_count; ++i)
        if (g_processes[i].state == State::Running) g_processes[i].ticks += ticks;
}

const Process* processes(uint32_t* count) {
    if (count) *count = g_count;
    return g_processes;
}

const Process* process_for(app_registry::AppId id) { return find(id); }
uint32_t process_count() { return g_count; }

const char* runtime_name(app_registry::Runtime r) {
    switch (r) {
        case app_registry::Runtime::Native: return "pea native";
        case app_registry::Runtime::X90Fusion: return "X90 Fusion";
        case app_registry::Runtime::WebSandbox: return "Web Sandbox";
        default: return "unknown";
    }
}
}
