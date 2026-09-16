#include "usb.hpp"

namespace {
static drivers::usb::Device g_devices[drivers::usb::kMaxDevices]{};
static uint32_t g_count = 0;
}

namespace drivers::usb {

void init() {
    g_count = 0;
    for (auto& device : g_devices) device = {};
}

const Device* devices(uint32_t* count) {
    if (count) *count = g_count;
    return g_devices;
}

uint32_t count() { return g_count; }

const char* speed_name(uint8_t speed) {
    switch (speed) {
        case 1: return "low-speed";
        case 2: return "full-speed";
        case 3: return "high-speed";
        case 4: return "super-speed";
        case 5: return "super-speed-plus";
        default: return "unknown";
    }
}

}
