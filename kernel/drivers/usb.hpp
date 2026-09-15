#pragma once
#include <stdint.h>

namespace drivers::usb {

struct Device {
    uint8_t address;
    uint16_t vendor;
    uint16_t product;
    uint8_t device_class;
    uint8_t subclass;
    uint8_t protocol;
    uint8_t speed;
    bool configured;
};

constexpr uint32_t kMaxDevices = 64;

void init();
const Device* devices(uint32_t* count);
uint32_t count();
const char* speed_name(uint8_t speed);

}
