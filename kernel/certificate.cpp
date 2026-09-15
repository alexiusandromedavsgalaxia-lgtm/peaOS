#include "certificate.hpp"

namespace certificate {

void init() {}

bool expired(const Certificate* cert, uint64_t now) {
    if (!cert || cert->magic != kMagic || cert->version != kVersion) return true;
    return now >= cert->expires_at;
}

bool can_add_application(const Certificate* cert, uint8_t current_count) {
    if (!cert || cert->magic != kMagic || cert->version != kVersion) return false;
    if (current_count >= kMaxApplications) return false;
    if (cert->application_count >= kMaxApplications) return false;
    return true;
}

Status validate(const Certificate* cert, uint64_t now, uint8_t app_count,
                Distribution distribution) {
    if (!cert || cert->magic != kMagic || cert->version != kVersion) {
        return Status::Invalid;
    }
    if (cert->application_count == 0 || cert->application_count > kMaxApplications ||
        app_count > kMaxApplications) {
        return Status::ApplicationLimit;
    }
    if (now < cert->issued_at || now >= cert->expires_at ||
        cert->expires_at <= cert->issued_at ||
        cert->expires_at - cert->issued_at > kValiditySeconds) {
        return Status::Expired;
    }
    // A web-distributed package must carry a certificate explicitly issued
    // for Web Distribution. A local certificate cannot be promoted by the
    // installer after the package has left the trusted local path.
    if (distribution != static_cast<Distribution>(cert->distribution)) {
        return Status::WebDistributionRequired;
    }
    return Status::Valid;
}

}
