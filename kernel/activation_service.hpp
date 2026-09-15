#pragma once

#include <stdint.h>

namespace activation_service {

struct ActivationRequest {
    const uint8_t* package_digest;
    uint32_t package_digest_size;
    const uint8_t* certificate;
    uint32_t certificate_size;
    const uint8_t* web_origin;
    uint32_t web_origin_size;
};

bool validate_request(const ActivationRequest& request);

}
