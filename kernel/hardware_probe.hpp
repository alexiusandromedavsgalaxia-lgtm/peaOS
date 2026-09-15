#pragma once

#include <stdint.h>
#include "hardware_requirements.hpp"

namespace hardware_probe {

struct Result {
    hardware::Profile profile;
    bool cpuid_available;
    bool long_mode_available;
    char vendor[13];
};

Result probe(uint64_t multiboot_info);

}
