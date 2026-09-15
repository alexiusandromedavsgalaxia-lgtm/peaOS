#pragma once
#include <stdint.h>

namespace development {

enum class Target : uint8_t { PeaOS, Windows, Android, MacOS };
enum class MacState : uint8_t { RequiresMac, WaitingForXcode, Ready, Disconnected, BuildRunning, BuildComplete };

struct Project {
    Target target;
    const char* name;
    const char* package_extension;
    bool live_sync;
};

struct MacLink {
    MacState state;
    bool usb_c_connected;
    bool xcode_open;
    uint32_t synced_files;
};

void init();
Project create_project(Target target, const char* name);
const char* target_name(Target target);
const char* target_extension(Target target);
bool target_available(Target target);
void set_mac_connection(bool connected, bool xcode_open);
MacLink mac_link();
bool request_build(Target target);
const char* status();

}
