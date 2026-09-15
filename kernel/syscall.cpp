#include "syscall.hpp"
#include "console.hpp"
#include "memory.hpp"
#include "hardware_probe.hpp"

namespace syscall_api {

namespace {
uint64_t g_next_pid = 1;
}

uint64_t dispatch(uint64_t number, uint64_t arg0, uint64_t arg1, uint64_t arg2) {
    switch (static_cast<Number>(number)) {
        case Number::Exit:
            return 0;
        case Number::Write:
            if (arg0 == 0 || arg1 > 4096) return static_cast<uint64_t>(-1);
            console::write(reinterpret_cast<const char*>(arg0));
            return 0;
        case Number::GetTime:
            return 0;
        case Number::GetProcessId:
            return g_next_pid++;
        case Number::GetSystemInfo: {
            if (arg0 == 0 || arg1 < sizeof(SystemInfo)) return static_cast<uint64_t>(-1);
            SystemInfo* out = reinterpret_cast<SystemInfo*>(arg0);
            const hardware_probe::Result probe = hardware_probe::probe(0);
            out->abi_version = kAbiVersion;
            out->cpu_count = probe.profile.logical_cpus;
            out->ram_bytes = probe.profile.ram_bytes;
            out->features = 0;
            (void)arg2;
            return 0;
        }
    }
    return static_cast<uint64_t>(-1);
}

}
