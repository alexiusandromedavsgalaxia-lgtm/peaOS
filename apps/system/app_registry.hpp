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

struct AppDescriptor {
    AppId id;
    const char* name;
    const char* package_extension;
    bool bundled;
    bool requires_network;
    bool isolated;
};

const AppDescriptor* all(uint32_t* count);
const AppDescriptor* find(AppId id);
const AppDescriptor* find_by_name(const char* name);

}
