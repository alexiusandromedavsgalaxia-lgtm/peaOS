#include "app_registry.hpp"

namespace app_registry {
namespace {
constexpr AppDescriptor kApps[] = {
    {AppId::Settings, "Settings", ".pea", Runtime::Native, Category::System, true, false, false},
    {AppId::Files, "Files", ".pea", Runtime::Native, Category::System, true, false, false},
    {AppId::SoftwareCenter, "Software Center", ".pea", Runtime::Native, Category::System, true, true, true},
    {AppId::Terminal, "Terminal", ".pea", Runtime::Native, Category::System, true, false, true},
    {AppId::TaskManager, "Task Manager", ".pea", Runtime::Native, Category::System, true, false, true},
    {AppId::SystemMonitor, "System Monitor", ".pea", Runtime::Native, Category::System, true, false, true},
    {AppId::TextEditor, "Text Editor", ".pea", Runtime::Native, Category::Productivity, true, false, false},
    {AppId::MediaPlayer, "Media Player", ".pea", Runtime::Native, Category::Media, true, false, true},
    {AppId::Photos, "Photos", ".pea", Runtime::Native, Category::Media, true, false, false},
    {AppId::ImageViewer, "Image Viewer", ".pea", Runtime::Native, Category::Media, true, false, false},
    {AppId::Viewer, "Viewer", ".pea", Runtime::Native, Category::Productivity, true, false, false},
    {AppId::Browser, "Web Browser", ".pea", Runtime::X90Fusion, Category::Internet, true, true, true},
    {AppId::Notes, "Notes", ".pea", Runtime::Native, Category::Productivity, true, false, false},
    {AppId::Calendar, "Calendar", ".pea", Runtime::Native, Category::Productivity, true, false, false},
    {AppId::Clock, "Clock", ".pea", Runtime::Native, Category::System, true, false, false},
    {AppId::NetworkManager, "Network Manager", ".pea", Runtime::Native, Category::System, true, true, true},
    {AppId::Bluetooth, "Bluetooth", ".pea", Runtime::Native, Category::System, true, false, true},
    {AppId::Audio, "Audio", ".pea", Runtime::Native, Category::System, true, false, true},
    {AppId::Display, "Display", ".pea", Runtime::Native, Category::System, true, false, false},
    {AppId::UsersAccounts, "Users & Accounts", ".pea", Runtime::Native, Category::Security, true, false, true},
    {AppId::SecurityCenter, "Security Center", ".pea", Runtime::Native, Category::Security, true, true, true},
    {AppId::Permissions, "Permissions", ".pea", Runtime::Native, Category::Security, true, false, true},
    {AppId::Notifications, "Notifications", ".pea", Runtime::Native, Category::System, true, false, true},
    {AppId::About, "About peaOS", ".pea", Runtime::Native, Category::System, true, false, false},
    {AppId::Help, "Help", ".pea", Runtime::Native, Category::System, true, false, false},
    {AppId::PeaCloud, "peaCloud", ".pea", Runtime::Native, Category::Internet, true, true, true},
    {AppId::MangaReader, "Manga Reader", ".pea", Runtime::Native, Category::Media, true, true, true},
    {AppId::Development, "Development", ".pea", Runtime::Native, Category::Development, true, false, true},
    {AppId::VisualStudioCode, "Visual Studio Code", ".pea", Runtime::X90Fusion, Category::Development, true, false, true},
    {AppId::SandboxInterface, "Sandbox Interface", ".pea", Runtime::X90Fusion, Category::Security, true, false, true},
    {AppId::X90FusionManager, "X90 Fusion Manager", ".pea", Runtime::Native, Category::X90, true, false, true},
    {AppId::TimeMachineManager, "TimeMachine Manager", ".pea", Runtime::Native, Category::X90, true, false, true},
    {AppId::MorphManager, "Morph Manager", ".pea", Runtime::Native, Category::X90, true, false, false},
    {AppId::CertificateYourApp, "Certificate Your App", ".pea", Runtime::Native, Category::Security, true, true, true},
    {AppId::WebDistribution, "Web Distribution", ".pea", Runtime::Native, Category::Internet, true, true, true}
};

bool same(const char* a, const char* b) {
    if (!a || !b) return false;
    while (*a && *b && *a == *b) { ++a; ++b; }
    return *a == *b;
}
}

const AppDescriptor* all(uint32_t* count) {
    if (count) *count = static_cast<uint32_t>(sizeof(kApps) / sizeof(kApps[0]));
    return kApps;
}

const AppDescriptor* find(AppId id) {
    for (const AppDescriptor& app : kApps)
        if (app.id == id) return &app;
    return nullptr;
}

const AppDescriptor* find_by_name(const char* name) {
    for (const AppDescriptor& app : kApps)
        if (same(app.name, name)) return &app;
    return nullptr;
}

bool is_bundled(AppId id) { const AppDescriptor* app = find(id); return app && app->bundled; }
bool requires_network(AppId id) { const AppDescriptor* app = find(id); return app && app->requires_network; }
bool is_isolated(AppId id) { const AppDescriptor* app = find(id); return app && app->isolated; }

}
