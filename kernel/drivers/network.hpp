#pragma once
#include <stdint.h>

namespace drivers::network {

enum class Link : uint8_t { Down, Up, Unknown };
enum class Medium : uint8_t { Unknown, Ethernet, Wifi, BluetoothPAN, Cellular, Loopback };
enum class DriverKind : uint8_t { None, RTL8139, IntelE1000, VirtioNet, GenericEthernet };

struct Interface {
    uint32_t id;
    Medium medium;
    DriverKind driver;
    Link link;
    uint8_t mac[6];
    uint16_t mtu;
    bool ipv4;
    bool ipv6;
    bool dhcp;
    bool dns;
    bool active;
};

constexpr uint32_t kMaxInterfaces = 16;

void init();
uint32_t probe_pci();
const Interface* interfaces(uint32_t* count);
const Interface* interface_for(uint32_t id);
uint32_t count();
bool bring_up(uint32_t id);
bool bring_down(uint32_t id);
const char* driver_name(DriverKind driver);
const char* medium_name(Medium medium);
const char* link_name(Link link);

}
