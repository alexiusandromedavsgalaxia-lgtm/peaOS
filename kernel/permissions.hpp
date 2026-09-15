#pragma once
#include <stdint.h>

namespace permissions {

enum class Capability : uint8_t {
    Files = 0,
    Network = 1,
    Contacts = 2,
    Audio = 3,
    Camera = 4,
    Microphone = 5,
    Notifications = 6,
    Device = 7
};

struct Grant {
    uint64_t app_id;
    uint32_t capabilities;
};

constexpr uint32_t bit(Capability capability) {
    return 1u << static_cast<uint8_t>(capability);
}

void init();
bool request(uint64_t app_id, Capability capability);
bool revoke(uint64_t app_id, Capability capability);
bool allowed(uint64_t app_id, Capability capability);

}
