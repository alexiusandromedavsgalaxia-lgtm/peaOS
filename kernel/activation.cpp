#include "activation.hpp"

namespace activation {
namespace {
ActivationRecord g_record{};
DeviceIdentity g_device{};
State g_state = State::Unactivated;

void clear(uint8_t* p, uint32_t n) {
    if (!p) return;
    for (uint32_t i = 0; i < n; ++i) p[i] = 0;
}

bool nonempty(const uint8_t* p, uint32_t n) {
    return p != nullptr && n != 0;
}
}

void init() {
    clear(reinterpret_cast<uint8_t*>(&g_record), sizeof(g_record));
    clear(reinterpret_cast<uint8_t*>(&g_device), sizeof(g_device));
    g_record.magic = kMagic;
    g_record.version = kVersion;
    g_record.state = State::Unactivated;
    g_state = State::Unactivated;
}

State state() { return g_state; }
const ActivationRecord& record() { return g_record; }
const DeviceIdentity& device_identity() { return g_device; }

bool begin_online_activation(const uint8_t* token, uint32_t token_size,
                             const uint8_t* server_key, uint32_t server_key_size,
                             uint64_t now) {
    if (!nonempty(token, token_size) || !nonempty(server_key, server_key_size) || now == 0) {
        g_state = State::Failed;
        g_record.state = State::Failed;
        return false;
    }

    // The cryptographic verifier and network transport are deliberately not
    // faked here. This function only records that a verification request has
    // entered the privileged activation service.
    g_state = State::Activating;
    g_record.state = State::Activating;
    return true;
}

bool verify_online(uint64_t now) {
    if (g_state != State::Activating && g_state != State::Active) return false;
    if (now == 0) return false;

    // Never grant ACTIVE until a real server protocol, certificate-chain
    // verification and persistent device identity are implemented.
    return false;
}

void revoke() {
    g_state = State::Revoked;
    g_record.state = State::Revoked;
    clear(g_record.activation_token_hash, sizeof(g_record.activation_token_hash));
}
}
