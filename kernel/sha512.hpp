#pragma once

#include <stdint.h>

namespace sha512 {

constexpr uint8_t kDigestSize = 64;

void hash(const uint8_t* data, uint64_t length, uint8_t out[kDigestSize]);

}
