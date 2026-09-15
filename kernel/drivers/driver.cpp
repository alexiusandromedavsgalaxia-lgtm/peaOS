#include "driver.hpp"

namespace {
drivers::Device g_devices[drivers::kMaxDevices]{};
uint32_t g_count = 0;
}

namespace drivers {
void init() { g_count = 0; for (auto& d : g_devices) d = {}; }
bool register_device(const Device& device) { if (g_count >= kMaxDevices) return false; g_devices[g_count++] = device; return true; }
const Device* devices(uint32_t* count) { if (count) *count = g_count; return g_devices; }
const Device* find(uint16_t vendor, uint16_t device) { for (uint32_t i = 0; i < g_count; ++i) if (g_devices[i].vendor == vendor && g_devices[i].device == device) return &g_devices[i]; return nullptr; }
uint32_t count() { return g_count; }
const char* bus_name(Bus b) { switch (b) { case Bus::PCI: return "PCI"; case Bus::USB: return "USB"; case Bus::I2C: return "I2C"; case Bus::SPI: return "SPI"; case Bus::Platform: return "Platform"; default: return "Unknown"; } }
const char* class_name(Class c) { switch (c) { case Class::Network: return "Network"; case Class::Bluetooth: return "Bluetooth"; case Class::Storage: return "Storage"; case Class::Display: return "Display"; case Class::Input: return "Input"; case Class::Audio: return "Audio"; case Class::USB: return "USB"; case Class::Serial: return "Serial"; case Class::Power: return "Power"; case Class::Sensor: return "Sensor"; default: return "Unknown"; } }
const char* state_name(State s) { switch (s) { case State::Detected: return "detected"; case State::Probing: return "probing"; case State::Ready: return "ready"; case State::Failed: return "failed"; default: return "absent"; } }
}
