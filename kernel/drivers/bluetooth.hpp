#pragma once
#include <stdint.h>

namespace drivers::bluetooth {

enum class Transport : uint8_t { None, USB, UART, PCIe };
enum class State : uint8_t { Absent, Detected, Ready, Failed };

struct Adapter {
    uint16_t vendor;
    uint16_t product;
    Transport transport;
    State state;
    uint8_t address[6];
    bool hci;
    bool l2cap;
    bool hid;
    bool pan;
};

constexpr uint32_t kMaxAdapters = 16;

void init();
const Adapter* adapters(uint32_t* count);
uint32_t count();
const char* transport_name(Transport transport);
const char* state_name(State state);

}
