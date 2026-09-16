#include "xhci.hpp"
#include "pci.hpp"

namespace {
static drivers::xhci::Controller g_controller{};
static inline uint32_t read32(uint64_t address) { return *reinterpret_cast<volatile uint32_t*>(address); }
static inline void write32(uint64_t address, uint32_t value) { *reinterpret_cast<volatile uint32_t*>(address) = value; }
static void delay() { for (volatile uint32_t i = 0; i < 10000; ++i) asm volatile("pause"); }

uint64_t find_mmio(const drivers::pci::Device& d) {
    for (uint32_t i = 0; i < 6; ++i) {
        if (d.bar[i] & 1u) continue;
        const uint32_t type = (d.bar[i] >> 1) & 3u;
        if (type == 2 && i + 1 < 6) return (static_cast<uint64_t>(d.bar[i + 1]) << 32) | (d.bar[i] & ~0xFu);
        if (d.bar[i]) return d.bar[i] & ~0xFu;
    }
    return 0;
}
}

namespace drivers::xhci {

void init() { g_controller = {}; }

bool probe() {
    uint32_t count = 0;
    const pci::Device* devices = pci::devices(&count);
    for (uint32_t i = 0; i < count; ++i) {
        const pci::Device& d = devices[i];
        if (d.class_code != 0x0C || d.subclass != 0x03 || d.prog_if != 0x30) continue;
        const uint64_t base = find_mmio(d);
        if (!base) continue;
        g_controller = {};
        g_controller.pci_bus = d.bus;
        g_controller.pci_slot = d.slot;
        g_controller.pci_function = d.function;
        g_controller.mmio_base = base;
        g_controller.cap_length = static_cast<uint8_t>(read32(base) & 0xFFu);
        g_controller.max_ports = static_cast<uint8_t>((read32(base + 0x04) >> 24) & 0xFFu);
        g_controller.present = true;

        const uint64_t op = base + g_controller.cap_length;
        uint32_t command = read32(op + 0x00);
        command &= ~0x1u;             // halt before reset
        write32(op + 0x00, command);
        for (uint32_t t = 0; t < 1000; ++t) {
            if (read32(op + 0x04) & 0x1u) break;
            delay();
        }
        write32(op + 0x00, read32(op + 0x00) | 0x2u); // HCRST
        for (uint32_t t = 0; t < 10000; ++t) {
            if ((read32(op + 0x00) & 0x2u) == 0 && (read32(op + 0x04) & 0x800u) == 0) { g_controller.reset_ok = true; break; }
            delay();
        }
        if (!g_controller.reset_ok) return false;
        write32(op + 0x00, read32(op + 0x00) | 0x1u); // run
        for (uint32_t t = 0; t < 1000; ++t) {
            if (read32(op + 0x04) & 0x1u) { g_controller.running = true; break; }
            delay();
        }
        return g_controller.running;
    }
    return false;
}

const Controller* controller() { return &g_controller; }
bool available() { return g_controller.present && g_controller.reset_ok && g_controller.running; }
uint8_t port_count() { return g_controller.max_ports; }

}
