#pragma once
#include <stdint.h>

namespace syscall_api {

enum class Number : uint64_t {
    Exit = 0,
    Write = 1,
    GetTime = 2,
    GetProcessId = 3,
    GetSystemInfo = 4
};

struct SystemInfo {
    uint32_t abi_version;
    uint32_t cpu_count;
    uint64_t ram_bytes;
    uint64_t features;
};

constexpr uint32_t kAbiVersion = 1;

uint64_t dispatch(uint64_t number, uint64_t arg0, uint64_t arg1, uint64_t arg2);

}
