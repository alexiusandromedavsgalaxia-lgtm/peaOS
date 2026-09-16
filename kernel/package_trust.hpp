#pragma once

#include <stdint.h>
#include "certificate.hpp"

namespace package_trust {

enum class PackageKind : uint8_t { Pea, XPea, Peac, Foreign };
enum class Result : uint8_t { Rejected, AcceptedLocal, AcceptedWeb };

struct Package {
    PackageKind kind;
    const uint8_t* digest;
    uint32_t digest_size;
    const certificate::Certificate* certificate;
    const uint8_t* web_origin;
    uint32_t web_origin_size;
};

void init();
Result verify(const Package& package, uint64_t now);

}
