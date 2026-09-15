#include "pci.hpp"
#include "driver.hpp"

namespace {
static drivers::pci::Device g_devices[drivers::pci::kMaxDevices]{};
static uint32_t g_count = 0;

static inline void outl(uint16_t port, uint32_t value) { asm volatile("outl %0, %1" :: "a"(value), "Nd"(port)); }
static inline uint32_t inl(uint16_t port) { uint32_t value; asm volatile("inl %1, %0" : "=a"(value) : "Nd"(port)); return value; }

uint32_t config_read32(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset) {
    const uint32_t address = 0x80000000u |
        (static_cast<uint32_t>(bus) << 16) |
        (static_cast<uint32_t>(slot) << 11) |
        (static_cast<uint32_t>(function) << 8) |
        (offset & 0xFCu);
    outl(0xCF8, address);
    return inl(0xCFC);
}
}

namespace drivers::pci {

uint32_t read32(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset) { return config_read32(bus, slot, function, offset); }
uint16_t read16(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset) {
    const uint32_t value = config_read32(bus, slot, function, offset);
    const uint8_t shift = static_cast<uint8_t>((offset & 2u) * 8u);
    return static_cast<uint16_t>((value >> shift) & 0xFFFFu);
}

void init() { g_count = 0; for (auto& d : g_devices) d = {}; }

uint32_t enumerate() {
    g_count = 0;
    for (uint32_t bus = 0; bus < 256 && g_count < kMaxDevices; ++bus) {
        for (uint32_t slot = 0; slot < 32 && g_count < kMaxDevices; ++slot) {
            for (uint32_t function = 0; function < 8 && g_count < kMaxDevices; ++function) {
                const uint32_t id = config_read32(static_cast<uint8_t>(bus), static_cast<uint8_t>(slot), static_cast<uint8_t>(function), 0);
                if ((id & 0xFFFFu) == 0xFFFFu) {
                    if (function == 0) break;
                    continue;
                }
                const uint32_t class_info = config_read32(static_cast<uint8_t>(bus), static_cast<uint8_t>(slot), static_cast<uint8_t>(function), 8);
                const uint32_t irq = config_read32(static_cast<uint8_t>(bus), static_cast<uint8_t>(slot), static_cast<uint8_t>(function), 0x3C);
                Device& d = g_devices[g_count++];
                d.bus = static_cast<uint8_t>(bus); d.slot = static_cast<uint8_t>(slot); d.function = static_cast<uint8_t>(function);
                d.vendor = static_cast<uint16_t>(id & 0xFFFFu); d.device = static_cast<uint16_t>(id >> 16);
                d.class_code = static_cast<uint8_t>(class_info >> 24); d.subclass = static_cast<uint8_t>(class_info >> 16); d.prog_if = static_cast<uint8_t>(class_info >> 8);
                d.irq_line = static_cast<uint8_t>(irq & 0xFFu);
            }
        }
    }
    return g_count;
}

const Device* devices(uint32_t* count) { if (count) *count = g_count; return g_devices; }
uint32_t count() { return g_count; }

}
