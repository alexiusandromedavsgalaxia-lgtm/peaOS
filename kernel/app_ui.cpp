#include "app_ui.hpp"
#include "window_manager.hpp"

namespace app_ui {
namespace {
constexpr uint32_t kMaxSurfaces = 64;
AppSurface g_surfaces[kMaxSurfaces]{};
uint32_t g_count = 0;
uint32_t g_width = 1024;
uint32_t g_height = 768;

const char* title(app_registry::AppId id) {
    const auto* app = app_registry::find(id);
    return app ? app->name : "Unknown application";
}
View default_view(app_registry::AppId id) {
    switch (id) {
        case app_registry::AppId::Settings:
        case app_registry::AppId::Display:
        case app_registry::AppId::Audio:
        case app_registry::AppId::UsersAccounts:
        case app_registry::AppId::Permissions: return View::Settings;
        case app_registry::AppId::Files:
        case app_registry::AppId::SoftwareCenter:
        case app_registry::AppId::Notes:
        case app_registry::AppId::MangaReader: return View::List;
        case app_registry::AppId::Photos:
        case app_registry::AppId::ImageViewer: return View::Grid;
        case app_registry::AppId::TextEditor:
        case app_registry::AppId::Development:
        case app_registry::AppId::VisualStudioCode: return View::Editor;
        case app_registry::AppId::TaskManager:
        case app_registry::AppId::SystemMonitor: return View::Monitor;
        case app_registry::AppId::Browser:
        case app_registry::AppId::WebDistribution: return View::Browser;
        case app_registry::AppId::MediaPlayer: return View::Player;
        case app_registry::AppId::Calendar: return View::Calendar;
        case app_registry::AppId::Clock: return View::Clock;
        case app_registry::AppId::NetworkManager: return View::Network;
        case app_registry::AppId::Bluetooth: return View::Bluetooth;
        case app_registry::AppId::SecurityCenter:
        case app_registry::AppId::CertificateYourApp:
        case app_registry::AppId::SandboxInterface: return View::Security;
        case app_registry::AppId::X90FusionManager:
        case app_registry::AppId::TimeMachineManager:
        case app_registry::AppId::MorphManager: return View::Development;
        case app_registry::AppId::About:
        case app_registry::AppId::Help: return View::About;
        default: return View::Home;
    }
}
int find_index(app_registry::AppId id) {
    for (uint32_t i = 0; i < g_count; ++i) if (g_surfaces[i].id == id) return static_cast<int>(i);
    return -1;
}
}

void init(uint32_t width, uint32_t height) {
    g_width = width ? width : 1024;
    g_height = height ? height : 768;
    g_count = 0;
    for (auto& s : g_surfaces) s = {};
}

const AppSurface* surface(app_registry::AppId id) {
    const int i = find_index(id);
    return i >= 0 ? &g_surfaces[i] : nullptr;
}

bool open(app_registry::AppId id) {
    if (!app_registry::find(id) || surface(id) || g_count >= kMaxSurfaces) return false;
    AppSurface& s = g_surfaces[g_count++];
    s.id = id;
    s.title = title(id);
    s.subtitle = "peaOS system application";
    s.view = default_view(id);
    s.bounds = {80, 64, g_width > 160 ? g_width - 160 : g_width, g_height > 128 ? g_height - 128 : g_height};
    s.focused = true;
    s.dirty = true;
    for (uint32_t i = 0; i + 1 < g_count; ++i) g_surfaces[i].focused = false;
    return true;
}

bool close(app_registry::AppId id) {
    const int index = find_index(id);
    if (index < 0) return false;
    const uint32_t last = g_count - 1;
    if (static_cast<uint32_t>(index) != last) g_surfaces[index] = g_surfaces[last];
    g_surfaces[last] = {};
    --g_count;
    return true;
}

bool focus(app_registry::AppId id) {
    const int index = find_index(id);
    if (index < 0) return false;
    for (uint32_t i = 0; i < g_count; ++i) g_surfaces[i].focused = (static_cast<int>(i) == index);
    g_surfaces[index].dirty = true;
    return true;
}

bool dispatch(app_registry::AppId id, const Event& event) {
    const int index = find_index(id);
    if (index < 0) return false;
    AppSurface& s = g_surfaces[index];
    switch (event.type) {
        case EventType::PointerDown: focus(id); s.dirty = true; return true;
        case EventType::Back: s.dirty = true; return true;
        case EventType::Close: return close(id);
        default: s.dirty = true; return true;
    }
}

void tick(uint64_t ticks) {
    (void)ticks;
    for (uint32_t i = 0; i < g_count; ++i) if (g_surfaces[i].focused) g_surfaces[i].dirty = true;
}

uint32_t open_count() { return g_count; }
const AppSurface* surfaces(uint32_t* count) { if (count) *count = g_count; return g_surfaces; }

const char* view_name(View view) {
    switch (view) {
        case View::Home: return "home";
        case View::List: return "list";
        case View::Grid: return "grid";
        case View::Editor: return "editor";
        case View::Settings: return "settings";
        case View::Monitor: return "monitor";
        case View::Browser: return "browser";
        case View::Player: return "player";
        case View::Calendar: return "calendar";
        case View::Clock: return "clock";
        case View::Network: return "network";
        case View::Bluetooth: return "bluetooth";
        case View::Security: return "security";
        case View::Development: return "development";
        case View::About: return "about";
    }
    return "unknown";
}
}
