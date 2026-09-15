#include "syscall.hpp"
#include "console.hpp"
#include "memory.hpp"
#include "hardware_probe.hpp"

namespace syscall_api {

namespace {
uint64_t g_next_pid = 1;
}

uint64_t dispatch(uint64_t number, uint64_t arg0, uint64_t arg1, uint64_t arg2) {
    (void)arg0;
    (void)arg1;
    (void)arg2;

    switch (static_cast<Number>(number)) {
        case Number::Exit:
            return 0;
        case Number::Write:
            // No user-address validation or user/kernel address-space split
            // exists yet. Never dereference an arbitrary caller pointer.
            return static_cast<uint64_t>(-1);
        case Number::GetTime:
            return 0;
        case Number::GetProcessId:
            return g_next_pid++;
        case Number::GetSystemInfo:
            // Same fail-closed rule: copying to an arbitrary pointer would be
            // an immediate kernel memory-corruption primitive.
            return static_cast<uint64_t>(-1);
    }
    return static_cast<uint64_t>(-1);
}

}
