#include "bluetooth.hpp"

namespace {
drivers::bluetooth::Adapter g_adapters[drivers::bluetooth::kMaxAdapters]{};
uint32_t g_count = 0;
}

namespace drivers::bluetooth {
void init() { g_count = 0; for (auto& a : g_adapters) a = {}; }
const Adapter* adapters(uint32_t* count) { if (count) *count = g_count; return g_adapters; }
uint32_t count() { return g_count; }
const char* transport_name(Transport t) { switch (t) { case Transport::USB: return "USB HCI"; case Transport::UART: return "UART HCI"; case Transport::PCIe: return "PCIe HCI"; default: return "None"; } }
const char* state_name(State s) { switch (s) { case State::Detected: return "detected"; case State::Ready: return "ready"; case State::Failed: return "failed"; default: return "absent"; } }
}
