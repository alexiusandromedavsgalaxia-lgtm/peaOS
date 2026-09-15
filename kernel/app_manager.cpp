#include "app_manager.hpp"
#include "desktop.hpp"

namespace app_manager {
namespace {
constexpr uint32_t kMaxRunning = 32;
RunningApp g_running[kMaxRunning]{};
uint32_t g_count = 0;
const char* g_status = "Application manager ready.";

desktop::App to_desktop(app_registry::AppId id) {
    return static_cast<desktop::App>(static_cast<uint8_t>(id));
}
}

void init() {
    for (uint32_t i = 0; i < kMaxRunning; ++i) g_running[i] = {};
    g_count = 0;
    g_status = "Application manager ready.";
}

bool launch(app_registry::AppId id) {
    const app_registry::AppDescriptor* app = app_registry::find(id);
    if (!app || !app->bundled) {
        g_status = "Application is not installed.";
        return false;
    }
    for (uint32_t i = 0; i < kMaxRunning; ++i) {
        if (g_running[i].running && g_running[i].id == id) {
            desktop::focus(g_running[i].window_id);
            g_status = "Application already running; focused existing window.";
            return true;
        }
    }
    if (g_count >= kMaxRunning) {
        g_status = "Application limit reached.";
        return false;
    }
    const uint32_t window = desktop::open(to_desktop(id));
    if (!window) {
        g_status = "Unable to create application window.";
        return false;
    }
    for (uint32_t i = 0; i < kMaxRunning; ++i) {
        if (!g_running[i].running) {
            g_running[i] = {id, window, true};
            ++g_count;
            g_status = "Application launched.";
            return true;
        }
    }
    desktop::close(window);
    g_status = "Unable to register application process.";
    return false;
}

bool close(app_registry::AppId id) {
    for (uint32_t i = 0; i < kMaxRunning; ++i) {
        if (g_running[i].running && g_running[i].id == id) {
            desktop::close(g_running[i].window_id);
            g_running[i].running = false;
            g_running[i].window_id = 0;
            if (g_count) --g_count;
            g_status = "Application closed.";
            return true;
        }
    }
    g_status = "Application is not running.";
    return false;
}

const RunningApp* running(uint32_t* count) {
    if (count) *count = g_count;
    return g_running;
}

uint32_t running_count() { return g_count; }
const char* last_status() { return g_status; }

}
