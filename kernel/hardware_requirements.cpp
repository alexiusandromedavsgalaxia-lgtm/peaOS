#include "hardware_requirements.hpp"

namespace hardware {

Report evaluate(const Profile& p) {
    Report r{};
    r.firmware = p.firmware_valid ? Check::Pass : Check::Fail;
    r.processor = p.cpu_supported ? Check::Pass : Check::Fail;
    r.memory = p.ram_bytes >= kMinimumRamBytes ? Check::Pass : Check::Fail;
    r.storage = (p.storage_valid && p.storage_bytes >= kMinimumStorageBytes)
        ? Check::Pass : Check::Fail;
    // A dedicated GPU is deliberately not required. Integrated graphics are valid.
    r.graphics = p.has_graphics ? Check::Pass : Check::Warning;

    r.hardware_looks_legitimate = p.firmware_valid && p.storage_valid && p.cpu_supported;
    r.meets_minimums = r.hardware_looks_legitimate &&
                        r.memory == Check::Pass &&
                        r.storage == Check::Pass;
    return r;
}

}
