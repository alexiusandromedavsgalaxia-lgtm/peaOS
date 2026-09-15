#pragma once
#include <stdint.h>

namespace development {

enum class Target : uint8_t { PeaOS, Windows, Android, MacOS };
enum class MacState : uint8_t { RequiresMac, WaitingForXcode, Ready, Disconnected, BuildRunning, BuildComplete };
enum class AndroidState : uint8_t { Unavailable, LocalIsolatedReady, BuildRunning, BuildComplete, Failed };

enum class WindowsToolchain : uint8_t { Unavailable, MSVC, LLVMClang, MinGW }; 

struct Project {
    Target target;
    const char* name;
    const char* package_extension;
    bool live_sync;
    bool isolated_workspace;
};

struct MacLink {
    MacState state;
    bool usb_c_connected;
    bool xcode_open;
    uint32_t synced_files;
};

struct AndroidWorkspace {
    AndroidState state;
    bool virtualized;
    bool visible;
    bool host_filesystem_shared;
};

struct WindowsLink {
    WindowsToolchain toolchain;
    bool official;
    bool installed;
};

void init();
Project create_project(Target target, const char* name);
const char* target_name(Target target);
const char* target_extension(Target target);
bool target_available(Target target);
void set_toolchain_ready(Target target, bool ready);
void set_windows_toolchain(WindowsToolchain toolchain, bool installed);
WindowsLink windows_toolchain();
void set_android_workspace(bool ready, bool visible);
AndroidWorkspace android_workspace();
void set_mac_connection(bool connected, bool xcode_open);
MacLink mac_link();
bool request_build(Target target);
const char* status();

}
