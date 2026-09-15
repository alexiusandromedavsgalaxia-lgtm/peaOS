#include "app_manager.hpp"
#include "window_manager.hpp"
#include "app_runtime.hpp"
#include "drivers/network.hpp"

namespace app_manager {
namespace {
constexpr uint32_t kMaxRunning = 32;
RunningApp g_running[kMaxRunning]{};
uint32_t g_count = 0;
const char* g_status = "Application manager ready.";
}

void init() {
    for (auto& r : g_running) r = {};
    g_count = 0;
    g_status = "Application manager ready.";
}

bool launch(app_registry::AppId id) {
    const auto* app = app_registry::find(id);
    if (!app || !app->bundled) {
        g_status = "Application is not installed.";
        return false;
    }
    for (auto& r : g_running) if (r.running && r.id == id) {
        window_manager::focus(r.window_id);
        g_status = "Application already running; focused existing window.";
        return true;
    }
    if (g_count >= kMaxRunning) {
        g_status = "Application limit reached.";
        return false;
    }

    const bool network_available = drivers::network::available();
    const auto result = app_runtime::start(id, network_available);
    if (result != app_runtime::Result::Ok) {
        g_status = (result == app_runtime::Result::NetworkRequired)
            ? "Application requires a real network link, but none is available."
            : "Application runtime refused launch.";
        return false;
    }

    const uint32_t window = window_manager::open(static_cast<desktop::App>(static_cast<uint8_t>(id)));
    if (!window) {
        app_runtime::stop(id);
        g_status = "Unable to create application window.";
        return false;
    }
    for (auto& r : g_running) if (!r.running) {
        r = {id, window, true};
        ++g_count;
        g_status = "Application launched; execution remains kernel-managed until user-mode isolation is available.";
        return true;
    }
    window_manager::close(window);
    app_runtime::stop(id);
    g_status = "Unable to register application process.";
    return false;
}

bool close(app_registry::AppId id) {
    for (auto& r : g_running) if (r.running && r.id == id) {
        window_manager::close(r.window_id);
        app_runtime::stop(id);
        r.running = false;
        r.window_id = 0;
        if (g_count) --g_count;
        g_status = "Application closed.";
        return true;
    }
    g_status = "Application is not running.";
    return false;
}

const RunningApp* running(uint32_t* count) { if (count) *count = g_count; return g_running; }
uint32_t running_count() { return g_count; }
const char* last_status() { return g_status; }
}
