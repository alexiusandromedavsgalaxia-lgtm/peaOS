#pragma once

#include <stdint.h>

namespace certificate {

constexpr uint32_t kMagic = 0x50434150; // "PCAP"
constexpr uint16_t kVersion = 1;
constexpr uint8_t kMaxApplications = 5;
constexpr uint64_t kValiditySeconds = 90ull * 24ull * 60ull * 60ull;

enum class Distribution : uint8_t {
    Local = 0,
    WebDistribution = 1,
};

enum class Status : uint8_t {
    Valid = 0,
    Missing = 1,
    Invalid = 2,
    Expired = 3,
    ApplicationLimit = 4,
    WebDistributionRequired = 5,
};

struct Certificate {
    uint32_t magic;
    uint16_t version;
    uint16_t reserved;
    uint64_t issued_at;
    uint64_t expires_at;
    uint8_t application_count;
    uint8_t distribution;
    uint8_t reserved2[6];
    uint8_t certificate_id[16];
    uint8_t issuer_key_id[16];
    uint8_t signature[64];
};

struct ApplicationBinding {
    uint8_t package_hash[32];
};

void init();
Status validate(const Certificate* cert, uint64_t now, uint8_t app_count,
                Distribution distribution);
bool expired(const Certificate* cert, uint64_t now);
bool can_add_application(const Certificate* cert, uint8_t current_count);

}
