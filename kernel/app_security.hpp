#pragma once

#include <stdint.h>

namespace app_security {

enum class Permission : uint8_t {
    Files,
    Network,
    Contacts,
    Audio,
    Camera,
    Microphone,
    Notifications,
    Bluetooth,
    Display,
    System
};

enum class Decision : uint8_t {
    Deny,
    Allow
};

struct Request {
    const uint8_t* package_digest;
    uint32_t package_digest_size;
    Permission permission;
};

void init();
Decision authorize(const Request& request, bool user_explicitly_allowed);

}
