#pragma once
#include <stdint.h>

namespace app_registry {

enum class AppId : uint16_t {
    Settings,
    Photos,
    Viewer,
    Browser,
    Notes,
    PeaCloud,
    MangaReader,
    Development,
    VisualStudioCode,
    SandboxInterface
};

enum class Runtime : uint8_t { Native, X90Fusion, WebSandbox };

struct AppDescriptor {
    AppId id;
    const char* name;
    const char* package_extension;
    Runtime runtime;
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
