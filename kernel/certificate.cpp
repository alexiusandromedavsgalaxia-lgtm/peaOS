#include "certificate.hpp"

namespace certificate {
namespace {

bool all_zero(const uint8_t* data, uint32_t size) {
    if (!data) return true;
    for (uint32_t i = 0; i < size; ++i)
        if (data[i] != 0) return false;
    return true;
}

bool valid_distribution(uint8_t value) {
    return value == static_cast<uint8_t>(Distribution::Local) ||
           value == static_cast<uint8_t>(Distribution::WebDistribution);
}

}

void init() {}

bool expired(const Certificate* cert, uint64_t now) {
    if (!cert || cert->magic != kMagic || cert->version != kVersion || now == 0) return true;
    if (cert->expires_at <= cert->issued_at) return true;
    return now >= cert->expires_at;
}

bool can_add_application(const Certificate* cert, uint8_t current_count) {
    if (!cert || cert->magic != kMagic || cert->version != kVersion) return false;
    if (cert->application_count == 0 || cert->application_count > kMaxApplications) return false;
    if (current_count >= kMaxApplications || current_count >= cert->application_count) return false;
    return true;
}

Status validate(const Certificate* cert, uint64_t now, uint8_t app_count,
                Distribution distribution) {
    // Five deliberate fail-closed checks are kept here:
    // 1. identity/version must match the peaOS certificate format;
    // 2. certificate identity and issuer identity cannot be empty;
    // 3. the declared application allowance and requested count are bounded;
    // 4. lifetime must be positive, <= 90 days, and contain the current time;
    // 5. distribution must be an actual enum value and match the certificate.
    if (!cert || cert->magic != kMagic || cert->version != kVersion) {
        return Status::Invalid;
    }
    if (all_zero(cert->certificate_id, sizeof(cert->certificate_id)) ||
        all_zero(cert->issuer_key_id, sizeof(cert->issuer_key_id)) ||
        all_zero(cert->signature, sizeof(cert->signature))) {
        return Status::Invalid;
    }
    if (cert->application_count == 0 || cert->application_count > kMaxApplications ||
        app_count == 0 || app_count > cert->application_count ||
        app_count > kMaxApplications) {
        return Status::ApplicationLimit;
    }
    if (now == 0 || cert->expires_at <= cert->issued_at ||
        cert->expires_at - cert->issued_at > kValiditySeconds ||
        now < cert->issued_at || now >= cert->expires_at) {
        return Status::Expired;
    }
    if (!valid_distribution(cert->distribution) ||
        distribution != static_cast<Distribution>(cert->distribution)) {
        return Status::WebDistributionRequired;
    }
    return Status::Valid;
}

}
