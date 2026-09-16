#include "network.hpp"
#include "pci.hpp"

namespace {
static drivers::network::Interface g_interfaces[drivers::network::kMaxInterfaces]{};
static uint32_t g_count = 0;

static inline void outb(uint16_t port, uint8_t value) { asm volatile("outb %0, %1" :: "a"(value), "Nd"(port)); }
static inline uint8_t inb(uint16_t port) { uint8_t value; asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port)); return value; }
static inline void outw(uint16_t port, uint16_t value) { asm volatile("outw %0, %1" :: "a"(value), "Nd"(port)); }
static inline uint16_t inw(uint16_t port) { uint16_t value; asm volatile("inw %1, %0" : "=a"(value) : "Nd"(port)); return value; }
static inline void outl(uint16_t port, uint32_t value) { asm volatile("outl %0, %1" :: "a"(value), "Nd"(port)); }
static inline uint32_t inl(uint16_t port) { uint32_t value; asm volatile("inl %1, %0" : "=a"(value) : "Nd"(port)); return value; }
static inline uint32_t mmio_read32(uint64_t address) { return *reinterpret_cast<volatile uint32_t*>(address); }
static inline void mmio_write32(uint64_t address, uint32_t value) { *reinterpret_cast<volatile uint32_t*>(address) = value; }
static inline void io_wait() { asm volatile("outb %%al, $0x80" :: "a"(0)); }

alignas(256) static uint8_t rtl_rx_buffer[8192 + 16];
struct E1000Desc { uint64_t address; uint16_t length; uint16_t checksum; uint8_t status; uint8_t errors; uint16_t special; };
alignas(16) static E1000Desc e1000_rx[16];
alignas(16) static E1000Desc e1000_tx[16];
alignas(16) static uint8_t e1000_rx_buffers[16][2048];

uint32_t bar_io(const drivers::pci::Device& d) {
    for (uint32_t i = 0; i < 6; ++i) if ((d.bar[i] & 1u) != 0) return d.bar[i] & ~3u;
    return 0;
}
uint64_t bar_mmio(const drivers::pci::Device& d) {
    for (uint32_t i = 0; i < 6; ++i) {
        if ((d.bar[i] & 1u) != 0) continue;
        const uint32_t type = (d.bar[i] >> 1) & 3u;
        if (type == 2 && i + 1 < 6) return (static_cast<uint64_t>(d.bar[i + 1]) << 32) | (d.bar[i] & ~0xFu);
        if (d.bar[i] != 0) return d.bar[i] & ~0xFu;
    }
    return 0;
}

bool init_rtl8139(drivers::network::Interface& n) {
    const uint16_t io = static_cast<uint16_t>(n.io_base);
    if (!io) return false;
    const uint16_t command = drivers::pci::read16(n.pci_bus, n.pci_slot, n.pci_function, 0x04);
    drivers::pci::write16(n.pci_bus, n.pci_slot, n.pci_function, 0x04, command | 0x0006u);
    outb(io + 0x52, 0x00);
    outb(io + 0x37, 0x10);
    for (uint32_t i = 0; i < 100000 && (inb(io + 0x37) & 0x10); ++i) io_wait();
    if (inb(io + 0x37) & 0x10) return false;
    for (uint32_t i = 0; i < 6; ++i) n.mac[i] = inb(io + static_cast<uint16_t>(i));
    const uint32_t rx = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(rtl_rx_buffer));
    outl(io + 0x30, rx);
    outw(io + 0x3C, 0x0005);
    outw(io + 0x44, 0x000Fu);
    outl(io + 0x40, 0x03000700u);
    outb(io + 0x37, 0x0C);
    const uint8_t media = inb(io + 0x58);
    n.link = (media & 0x04u) ? drivers::network::Link::Up : drivers::network::Link::Down;
    n.state = drivers::network::State::Initialized;
    n.active = n.link == drivers::network::Link::Up;
    return true;
}

bool init_e1000(drivers::network::Interface& n) {
    const uint64_t mmio = n.mmio_base;
    if (!mmio) return false;
    const uint16_t command = drivers::pci::read16(n.pci_bus, n.pci_slot, n.pci_function, 0x04);
    drivers::pci::write16(n.pci_bus, n.pci_slot, n.pci_function, 0x04, command | 0x0006u);
    mmio_write32(mmio + 0x0000, 0x00000040u);
    for (volatile uint32_t i = 0; i < 100000; ++i) { if ((mmio_read32(mmio + 0x0000) & 0x40u) == 0) break; }
    const uint32_t ral = mmio_read32(mmio + 0x5400);
    const uint32_t rah = mmio_read32(mmio + 0x5404);
    n.mac[0] = static_cast<uint8_t>(ral); n.mac[1] = static_cast<uint8_t>(ral >> 8);
    n.mac[2] = static_cast<uint8_t>(ral >> 16); n.mac[3] = static_cast<uint8_t>(ral >> 24);
    n.mac[4] = static_cast<uint8_t>(rah); n.mac[5] = static_cast<uint8_t>(rah >> 8);
    mmio_write32(mmio + 0x02800, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(e1000_rx)));
    mmio_write32(mmio + 0x02804, 0); mmio_write32(mmio + 0x02808, 16 * sizeof(E1000Desc));
    mmio_write32(mmio + 0x02810, 0); mmio_write32(mmio + 0x02818, 15); mmio_write32(mmio + 0x02828, 0);
    for (uint32_t i = 0; i < 16; ++i) { e1000_rx[i] = {}; e1000_rx[i].address = reinterpret_cast<uintptr_t>(e1000_rx_buffers[i]); }
    mmio_write32(mmio + 0x03800, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(e1000_tx)));
    mmio_write32(mmio + 0x03804, 0); mmio_write32(mmio + 0x03808, 16 * sizeof(E1000Desc));
    mmio_write32(mmio + 0x03810, 0); mmio_write32(mmio + 0x03818, 15);
    for (uint32_t i = 0; i < 16; ++i) e1000_tx[i] = {};
    mmio_write32(mmio + 0x0100, 0x0000009Eu);
    mmio_write32(mmio + 0x0400, 0x0000003Au);
    mmio_write32(mmio + 0x0000, 0x00000070u);
    const bool link = (mmio_read32(mmio + 0x0008) & 0x02u) != 0;
    n.link = link ? drivers::network::Link::Up : drivers::network::Link::Down;
    n.state = drivers::network::State::Initialized;
    n.active = link;
    return true;
}

bool init_virtio(drivers::network::Interface& n) {
    const uint16_t io = static_cast<uint16_t>(n.io_base);
    if (!io) return false;
    const uint16_t command = drivers::pci::read16(n.pci_bus, n.pci_slot, n.pci_function, 0x04);
    drivers::pci::write16(n.pci_bus, n.pci_slot, n.pci_function, 0x04, command | 0x0006u);
    outb(io + 0x12, 0x00); outb(io + 0x12, 0x01); outb(io + 0x12, 0x03);
    const uint32_t host_features = inl(io + 0x00); (void)host_features;
    outl(io + 0x04, 0x00000000u); outw(io + 0x0E, 0);
    const uint16_t qsize = inw(io + 0x0C);
    if (qsize == 0) return false;
    for (uint32_t i = 0; i < 6; ++i) n.mac[i] = inb(io + 0x14 + static_cast<uint16_t>(i));
    const uint16_t link = inw(io + 0x14 + 6);
    outb(io + 0x12, 0x07);
    n.link = (link & 1u) ? drivers::network::Link::Up : drivers::network::Link::Unknown;
    n.state = drivers::network::State::Initialized;
    n.active = n.link == drivers::network::Link::Up;
    return n.active;
}

bool add(const drivers::pci::Device& d, uint32_t id) {
    if (g_count >= drivers::network::kMaxInterfaces) return false;
    drivers::network::DriverKind kind = drivers::network::DriverKind::None;
    if (d.vendor == 0x10EC && d.device == 0x8139) kind = drivers::network::DriverKind::RTL8139;
    else if (d.vendor == 0x8086 && (d.device == 0x100E || d.device == 0x100F || d.device == 0x10D3 || d.device == 0x1533)) kind = drivers::network::DriverKind::IntelE1000;
    else if (d.vendor == 0x1AF4 && d.device >= 0x1000 && d.device <= 0x107F) kind = drivers::network::DriverKind::VirtioNet;
    else return false;
    drivers::network::Interface& n = g_interfaces[g_count];
    n = {};
    n.id = id; n.medium = drivers::network::Medium::Ethernet; n.driver = kind;
    n.link = drivers::network::Link::Unknown; n.state = drivers::network::State::Probed; n.mtu = 1500;
    n.pci_bus = d.bus; n.pci_slot = d.slot; n.pci_function = d.function;
    n.io_base = bar_io(d); n.mmio_base = bar_mmio(d);
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
    for (uint32_t i = 0; i < pc; ++i) if (devices[i].class_code == 0x02) add(devices[i], i + 1);
    return g_count;
}

const Interface* interfaces(uint32_t* count) { if (count) *count = g_count; return g_interfaces; }
const Interface* interface_for(uint32_t id) { for (uint32_t i = 0; i < g_count; ++i) if (g_interfaces[i].id == id) return &g_interfaces[i]; return nullptr; }
uint32_t count() { return g_count; }

bool bring_up(uint32_t id) {
    for (uint32_t i = 0; i < g_count; ++i) {
        Interface& n = g_interfaces[i];
        if (n.id != id) continue;
        if (n.state == State::Initialized && n.active) return true;
        bool ok = false;
        if (n.driver == DriverKind::RTL8139) ok = init_rtl8139(n);
        else if (n.driver == DriverKind::IntelE1000) ok = init_e1000(n);
        else if (n.driver == DriverKind::VirtioNet) ok = init_virtio(n);
        if (!ok) { n.state = State::Failed; n.active = false; return false; }
        return n.active;
    }
    return false;
}

bool bring_down(uint32_t id) {
    for (uint32_t i = 0; i < g_count; ++i) if (g_interfaces[i].id == id) {
        g_interfaces[i].active = false;
        g_interfaces[i].link = Link::Down;
        g_interfaces[i].ipv4 = false; g_interfaces[i].ipv6 = false; g_interfaces[i].dhcp = false; g_interfaces[i].dns = false;
        return true;
    }
    return false;
}

bool available() { for (uint32_t i = 0; i < g_count; ++i) if (g_interfaces[i].active && g_interfaces[i].link == Link::Up) return true; return false; }

const char* driver_name(DriverKind driver) {
    switch (driver) { case DriverKind::RTL8139: return "Realtek RTL8139"; case DriverKind::IntelE1000: return "Intel E1000 family"; case DriverKind::VirtioNet: return "VirtIO network"; case DriverKind::GenericEthernet: return "Generic Ethernet"; default: return "None"; }
}
const char* medium_name(Medium medium) { switch (medium) { case Medium::Ethernet: return "Ethernet"; case Medium::Wifi: return "Wi-Fi"; case Medium::BluetoothPAN: return "Bluetooth PAN"; case Medium::Cellular: return "Cellular"; case Medium::Loopback: return "Loopback"; default: return "Unknown"; } }
const char* link_name(Link link) { switch (link) { case Link::Up: return "up"; case Link::Down: return "down"; default: return "unknown"; } }
const char* state_name(State state) { switch (state) { case State::Probed: return "probed"; case State::Initialized: return "initialized"; case State::Failed: return "failed"; default: return "unprobed"; } }

}
