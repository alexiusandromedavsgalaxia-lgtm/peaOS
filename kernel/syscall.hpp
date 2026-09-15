#pragma once
#include <stdint.h>

namespace syscall {
enum Number : uint32_t {
    Yield = 0,
    Write = 1,
    Exit = 2,
    Alloc = 3,
    Open = 4,
};
struct Result { int32_t value; int32_t error; };
}
