#pragma once

#include <stdint.h>

namespace hardware {

constexpr uint64_t kMinimumRamBytes = 2ull * 1024ull * 1024ull * 1024ull;
constexpr uint64_t kMinimumStorageBytes = 64ull * 1024ull * 1024ull * 1024ull;

struct Profile {
    uint64_t ram_bytes;
    uint64_t storage_bytes;
    uint32_t logical_cpus;
    uint32_t cpu_mhz;
    bool has_graphics;
    bool has_dedicated_gpu;
    bool firmware_valid;
    bool storage_valid;
    bool cpu_supported;
};

enum class Check : uint8_t {
    Pass,
    Warning,
    Fail
};

struct Report {
    Check firmware;
    Check processor;
    Check memory;
    Check storage;
    Check graphics;
    bool hardware_looks_legitimate;
    bool meets_minimums;
};

Report evaluate(const Profile& profile);

}
