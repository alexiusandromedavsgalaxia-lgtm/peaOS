#include "app_registry.hpp"

namespace app_registry {
namespace {
constexpr AppDescriptor kApps[] = {
    {AppId::Settings, "Settings", ".pea", true, false, false},
    {AppId::Photos, "Photos", ".pea", true, false, false},
    {AppId::Viewer, "Viewer", ".pea", true, false, false},
    {AppId::Browser, "Web Browser", ".pea", true, true, true},
    {AppId::Notes, "Notes", ".pea", true, false, false},
    {AppId::PeaCloud, "peaCloud", ".pea", true, true, true},
    {AppId::MangaReader, "Manga Reader", ".pea", true, true, true},
    {AppId::Development, "Development", ".pea", true, true, true},
    {AppId::VisualStudioCode, "Visual Studio Code", ".pea", true, true, true},
    {AppId::SandboxInterface, "Sandbox Interface", ".pea", true, false, true}
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

}
