#pragma once

#include <stdint.h>

namespace certificate_app {

struct CertificateRequest {
    const char* company_name;
    const char* application_name;
    const uint8_t* package_digest;
    uint8_t package_digest_size;
};

struct RenewalResult {
    bool success;
    uint8_t applications_bound;
    uint64_t expires_at;
};

constexpr uint8_t kFreeApplicationLimit = 5;
constexpr uint64_t kValiditySeconds = 90ull * 24ull * 60ull * 60ull;

bool supports_native_package(const char* extension);
RenewalResult renew_manually(const char* certificate_id, uint64_t now);

}
