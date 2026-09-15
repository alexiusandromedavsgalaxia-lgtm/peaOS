#pragma once

#include <stdint.h>

namespace activation {

enum class State : uint8_t {
    Unactivated,
    Activating,
    Active,
    OfflineGrace,
    Revoked,
    Failed
};

struct DeviceIdentity {
    uint8_t device_key_id[32];
    uint8_t device_fingerprint[32];
};

struct ActivationRecord {
    uint32_t magic;
    uint16_t version;
    State state;
    uint8_t reserved;
    uint8_t installation_id[32];
    uint8_t device_key_id[32];
    uint8_t activation_token_hash[32];
    uint64_t activated_at;
    uint64_t last_verified_at;
};

constexpr uint32_t kMagic = 0x50454141; // PEAA
constexpr uint16_t kVersion = 1;

void init();
State state();
const ActivationRecord& record();
const DeviceIdentity& device_identity();

// Called by the privileged activation service after online verification.
bool begin_online_activation(const uint8_t* token, uint32_t token_size,
                             const uint8_t* server_key, uint32_t server_key_size,
                             uint64_t now);

bool verify_online(uint64_t now);
void revoke();

}
