#pragma once

#include <stdint.h>

namespace certificate {

constexpr uint32_t kMagic = 0x50434150; // "PCAP"
constexpr uint16_t kVersion = 3;
constexpr uint64_t kValiditySeconds = 365ull * 24ull * 60ull * 60ull;
constexpr uint8_t kEd25519PublicKeySize = 32;
constexpr uint8_t kEd25519SignatureSize = 64;
constexpr uint8_t kSha512Size = 64;

enum class CertificateType : uint8_t { Signing = 1 };
enum class Status : uint8_t {
    Valid = 0, Missing = 1, Invalid = 2, Expired = 3, Revoked = 4,
    SignatureInvalid = 5, IssuerMismatch = 6, ServerUnavailable = 7,
    NotActiveOnServer = 8, DigestMismatch = 9
};

struct Certificate {
    uint32_t magic;
    uint16_t version;
    uint8_t type;
    uint8_t reserved;
    uint64_t issued_at;
    uint64_t expires_at;
    uint64_t serial;
    uint8_t certificate_id[16];
    uint8_t issuer_key_id[16];
    uint8_t public_key[kEd25519PublicKeySize];
    uint8_t payload_sha512[kSha512Size];
    uint8_t signature[kEd25519SignatureSize];
};

struct ServerValidation {
    bool reachable;
    bool active;
    bool revoked;
    bool signature_valid;
    bool issuer_valid;
    bool digest_valid;
};

void init();
Status validate(const Certificate* cert, uint64_t now);
Status validate_server(const Certificate* cert, const ServerValidation& server);
bool expired(const Certificate* cert, uint64_t now);
bool install(const Certificate* cert, uint64_t now);
bool active();
void invalidate();
const Certificate* current();

}
