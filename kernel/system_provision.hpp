#pragma once

#include <stdint.h>

namespace systemprovision {

constexpr uint8_t kR35DigestSize = 64;
constexpr uint8_t kCertificateIdSize = 16;
constexpr uint16_t kMaxEndpointLength = 192;
constexpr uint16_t kMaxPayloadLength = 8192;

enum class Result : uint8_t {
    Ok = 0,
    InvalidInput = 1,
    DigestMismatch = 2,
    CertificateMismatch = 3,
    Expired = 4,
    Revoked = 5
};

struct R35Record {
    uint8_t certificate_id[kCertificateIdSize];
    uint8_t payload_sha512[kR35DigestSize];
    uint64_t issued_at;
    uint64_t expires_at;
    bool revoked;
};

void init();

// Builds the read-only GitHub endpoint used by SystemProvision.
// Authentication is performed by verifying the SHA-512 fingerprint of the
// exact endpoint payload against the signed R35 registry record.
bool build_github_endpoint(const uint8_t certificate_id[kCertificateIdSize], char* out, uint16_t out_size);

// R35: verify the exact bytes returned by the endpoint before accepting them.
Result authenticate_endpoint(const uint8_t certificate_id[kCertificateIdSize],
                             const uint8_t* payload, uint64_t payload_length,
                             const R35Record* record, uint64_t now);

bool constant_time_equal(const uint8_t* a, const uint8_t* b, uint64_t length);

}
