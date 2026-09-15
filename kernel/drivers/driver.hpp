#pragma once
#include <stdint.h>

namespace drivers {

enum class Bus : uint8_t { Unknown, PCI, USB, I2C, SPI, Platform };
enum class Class : uint8_t { Unknown, Network, Bluetooth, Storage, Display, Input, Audio, USB, Serial, Power, Sensor };
enum class State : uint8_t { Absent, Detected, Probing, Ready, Failed };

struct Device {
    uint16_t vendor;
    uint16_t device;
    uint8_t class_code;
    uint8_t subclass;
    Bus bus;
    Class type;
    State state;
    const char* name;
};

constexpr uint32_t kMaxDevices = 128;

void init();
bool register_device(const Device& device);
const Device* devices(uint32_t* count);
const Device* find(uint16_t vendor, uint16_t device);
uint32_t count();
const char* bus_name(Bus bus);
const char* class_name(Class type);
const char* state_name(State state);

}
