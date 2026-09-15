#pragma once
#include <stdint.h>

namespace drivers::pci {

struct Device {
    uint8_t bus;
    uint8_t slot;
    uint8_t function;
    uint16_t vendor;
    uint16_t device;
    uint8_t class_code;
    uint8_t subclass;
    uint8_t prog_if;
    uint8_t irq_line;
};

constexpr uint32_t kMaxDevices = 256;

void init();
uint32_t enumerate();
const Device* devices(uint32_t* count);
uint32_t count();
uint32_t read32(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
uint16_t read16(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);

}
