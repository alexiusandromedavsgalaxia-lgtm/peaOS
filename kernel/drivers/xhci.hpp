#pragma once
#include <stdint.h>

namespace drivers::xhci {

struct Controller {
    uint8_t pci_bus;
    uint8_t pci_slot;
    uint8_t pci_function;
    uint64_t mmio_base;
    uint8_t cap_length;
    uint8_t max_ports;
    bool present;
    bool reset_ok;
    bool running;
};

void init();
bool probe();
const Controller* controller();
bool available();
uint8_t port_count();

}
