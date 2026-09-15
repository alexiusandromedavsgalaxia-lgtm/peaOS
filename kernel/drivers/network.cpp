#include "network.hpp"
#include "pci.hpp"

namespace {
static drivers::network::Interface g_interfaces[drivers::network::kMaxInterfaces]{};
static uint32_t g_count = 0;

bool add(uint16_t vendor, uint16_t device, uint32_t id) {
    if (g_count >= drivers::network::kMaxInterfaces) return false;
    drivers::network::Interface& n = g_interfaces[g_count];
    n = {};
    n.id = id;
    n.medium = drivers::network::Medium::Ethernet;
    n.link = drivers::network::Link::Unknown;
    n.mtu = 1500;
    if (vendor == 0x10EC && device == 0x8139) n.driver = drivers::network::DriverKind::RTL8139;
    else if (vendor == 0x8086 && (device == 0x100E || device == 0x100F || device == 0x10D3 || device == 0x1533)) n.driver = drivers::network::DriverKind::IntelE1000;
    else if (vendor == 0x1AF4 && device >= 0x1000 && device <= 0x107F) n.driver = drivers::network::DriverKind::VirtioNet;
    else return false;
    ++g_count;
    return true;
}
}

namespace drivers::network {

void init() { g_count = 0; for (auto& n : g_interfaces) n = {}; }

uint32_t probe_pci() {
    g_count = 0;
    uint32_t pc = 0;
    const pci::Device* devices = pci::devices(&pc);
    for (uint32_t i = 0; i < pc; ++i) {
        if (devices[i].class_code != 0x02) continue;
        add(devices[i].vendor, devices[i].device, i + 1);
    }
    return g_count;
}

const Interface* interfaces(uint32_t* count) { if (count) *count = g_count; return g_interfaces; }
const Interface* interface_for(uint32_t id) { for (uint32_t i = 0; i < g_count; ++i) if (g_interfaces[i].id == id) return &g_interfaces[i]; return nullptr; }
uint32_t count() { return g_count; }

bool bring_up(uint32_t id) {
    // Probe-only drivers cannot honestly report a link until controller registers,
    // DMA and link negotiation are implemented. Keep the interface inactive.
    for (uint32_t i = 0; i < g_count; ++i) {
        if (g_interfaces[i].id != id) continue;
        if (g_interfaces[i].driver == DriverKind::None) return false;
        g_interfaces[i].active = false;
        g_interfaces[i].link = Link::Unknown;
        return false;
    }
    return false;
}

bool bring_down(uint32_t id) {
    for (uint32_t i = 0; i < g_count; ++i) if (g_interfaces[i].id == id) {
        g_interfaces[i].active = false;
        g_interfaces[i].link = Link::Down;
        g_interfaces[i].ipv4 = false;
        g_interfaces[i].ipv6 = false;
        g_interfaces[i].dhcp = false;
        g_interfaces[i].dns = false;
        return true;
    }
    return false;
}

bool available() {
    for (uint32_t i = 0; i < g_count; ++i)
        if (g_interfaces[i].active && g_interfaces[i].link == Link::Up) return true;
    return false;
}

const char* driver_name(DriverKind driver) {
    switch (driver) { case DriverKind::RTL8139: return "Realtek RTL8139"; case DriverKind::IntelE1000: return "Intel E1000 family"; case DriverKind::VirtioNet: return "VirtIO network"; case DriverKind::GenericEthernet: return "Generic Ethernet"; default: return "None"; }
}
const char* medium_name(Medium medium) {
    switch (medium) { case Medium::Ethernet: return "Ethernet"; case Medium::Wifi: return "Wi-Fi"; case Medium::BluetoothPAN: return "Bluetooth PAN"; case Medium::Cellular: return "Cellular"; case Medium::Loopback: return "Loopback"; default: return "Unknown"; }
}
const char* link_name(Link link) { switch (link) { case Link::Up: return "up"; case Link::Down: return "down"; default: return "unknown"; } }

}
