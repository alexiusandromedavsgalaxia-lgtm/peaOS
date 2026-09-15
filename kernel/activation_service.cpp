#include "activation_service.hpp"

namespace activation_service {
namespace {
bool present(const uint8_t* data, uint32_t size) {
    return data != nullptr && size != 0;
}
}

bool validate_request(const ActivationRequest& request) {
    // This is intentionally fail-closed. Package signatures, certificate
    // chains and web-origin authorization are verified by the future trusted
    // crypto/network service, not guessed by the installer.
    if (!present(request.package_digest, request.package_digest_size)) return false;
    if (!present(request.certificate, request.certificate_size)) return false;
    if (!present(request.web_origin, request.web_origin_size)) return false;
    return false;
}
}
