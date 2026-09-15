#include "permissions.hpp"

namespace permissions {
namespace {
constexpr uint32_t kMaxGrants = 128;
Grant g_grants[kMaxGrants]{};
}

void init() {
    for (uint32_t i = 0; i < kMaxGrants; ++i) g_grants[i] = {};
}

bool request(uint64_t app_id, Capability capability) {
    if (app_id == 0) return false;
    const uint32_t mask = bit(capability);
    for (uint32_t i = 0; i < kMaxGrants; ++i) {
        if (g_grants[i].app_id == app_id) {
            g_grants[i].capabilities |= mask;
            return true;
        }
    }
    for (uint32_t i = 0; i < kMaxGrants; ++i) {
        if (g_grants[i].app_id == 0) {
            g_grants[i] = {app_id, mask};
            return true;
        }
    }
    return false;
}

bool revoke(uint64_t app_id, Capability capability) {
    const uint32_t mask = bit(capability);
    for (uint32_t i = 0; i < kMaxGrants; ++i) {
        if (g_grants[i].app_id == app_id) {
            g_grants[i].capabilities &= ~mask;
            return true;
        }
    }
    return false;
}

bool allowed(uint64_t app_id, Capability capability) {
    const uint32_t mask = bit(capability);
    for (uint32_t i = 0; i < kMaxGrants; ++i) {
        if (g_grants[i].app_id == app_id)
            return (g_grants[i].capabilities & mask) != 0;
    }
    return false;
}

}
