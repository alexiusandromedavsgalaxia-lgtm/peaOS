#pragma once
#include <stdint.h>

namespace app_registry {

enum class AppId : uint16_t {
    Settings, Files, SoftwareCenter, Terminal, TaskManager, SystemMonitor,
    TextEditor, MediaPlayer, Photos, ImageViewer, Viewer, Browser, Notes,
    Calendar, Clock, NetworkManager, Bluetooth, Audio, Display, UsersAccounts,
    SecurityCenter, Permissions, Notifications, About, Help, PeaCloud,
    MangaReader, Development, VisualStudioCode, SandboxInterface,
    X90FusionManager, TimeMachineManager, MorphManager, CertificateYourApp,
    WebDistribution
};

enum class Runtime : uint8_t { Native, X90Fusion, WebSandbox };

enum class Category : uint8_t { System, Productivity, Media, Internet, Development, Security, X90 };

struct AppDescriptor {
    AppId id;
    const char* name;
    const char* package_extension;
    Runtime runtime;
    Category category;
    bool bundled;
    bool requires_network;
    bool isolated;
};

const AppDescriptor* all(uint32_t* count);
const AppDescriptor* find(AppId id);
const AppDescriptor* find_by_name(const char* name);
bool is_bundled(AppId id);
bool requires_network(AppId id);
bool is_isolated(AppId id);

}
