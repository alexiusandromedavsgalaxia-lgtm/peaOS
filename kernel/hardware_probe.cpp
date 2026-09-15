#include "hardware_probe.hpp"

namespace hardware_probe {
namespace {

static void copy_vendor(char* out, uint32_t ebx, uint32_t edx, uint32_t ecx) {
    out[0] = static_cast<char>(ebx);
    out[1] = static_cast<char>(ebx >> 8);
    out[2] = static_cast<char>(ebx >> 16);
    out[3] = static_cast<char>(ebx >> 24);
    out[4] = static_cast<char>(edx);
    out[5] = static_cast<char>(edx >> 8);
    out[6] = static_cast<char>(edx >> 16);
    out[7] = static_cast<char>(edx >> 24);
    out[8] = static_cast<char>(ecx);
    out[9] = static_cast<char>(ecx >> 8);
    out[10] = static_cast<char>(ecx >> 16);
    out[11] = static_cast<char>(ecx >> 24);
    out[12] = '\0';
}

static bool cpuid_supported() {
#if defined(__x86_64__)
    uint64_t before = 0;
    uint64_t after = 0;
    asm volatile("pushfq; popq %0" : "=r"(before));
    const uint64_t toggled = before ^ (1ull << 21);
    asm volatile("pushq %0; popfq" :: "r"(toggled) : "cc");
    asm volatile("pushfq; popq %0" : "=r"(after));
    asm volatile("pushq %0; popfq" :: "r"(before) : "cc");
    return ((before ^ after) & (1ull << 21)) != 0;
#else
    return false;
#endif
}
}

Result probe(uint64_t multiboot_info) {
    Result r{};
    r.profile.cpu_supported = false;
    r.profile.firmware_valid = multiboot_info != 0;
    r.profile.storage_valid = false;
    r.profile.has_graphics = true;
    r.profile.has_dedicated_gpu = false;

    r.cpuid_available = cpuid_supported();
    if (r.cpuid_available) {
#if defined(__x86_64__)
        uint32_t max_basic = 0;
        uint32_t vendor_ebx = 0, vendor_ecx = 0, vendor_edx = 0;
        asm volatile("cpuid" : "=a"(max_basic), "=b"(vendor_ebx), "=c"(vendor_ecx), "=d"(vendor_edx) : "a"(0));
        copy_vendor(r.vendor, vendor_ebx, vendor_edx, vendor_ecx);

        if (max_basic >= 1) {
            uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
            asm volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(1));
            r.profile.logical_cpus = (ebx >> 16) & 0xffu;
            r.profile.cpu_supported = true;
        }

        uint32_t max_ext = 0;
        uint32_t ebx = 0, ecx = 0, edx = 0;
        asm volatile("cpuid" : "=a"(max_ext), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0x80000000u));
        if (max_ext >= 0x80000001u) {
            uint32_t eax = 0;
            asm volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0x80000001u));
            r.long_mode_available = (edx & (1u << 29)) != 0;
        }
#endif
    }

    // Multiboot1 info: mem_lower at +4 and mem_upper at +8, both KiB.
    if (multiboot_info != 0) {
        const uint32_t* info = reinterpret_cast<const uint32_t*>(multiboot_info);
        const uint64_t lower = static_cast<uint64_t>(info[1]) * 1024ull;
        const uint64_t upper = static_cast<uint64_t>(info[2]) * 1024ull;
        r.profile.ram_bytes = lower + upper;
    }

    return r;
}
}
