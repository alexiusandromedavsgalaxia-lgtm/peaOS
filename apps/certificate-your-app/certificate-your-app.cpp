#include "certificate-your-app.hpp"

namespace certificate_app {

static bool ends_with(const char* value, const char* suffix) {
    if (!value || !suffix) return false;
    const char* a = value;
    const char* b = suffix;
    while (*a) ++a;
    while (*b) ++b;
    while (a > value && b > suffix && *(a - 1) == *(b - 1)) {
        --a;
        --b;
    }
    return b == suffix;
}

bool supports_native_package(const char* extension) {
    return ends_with(extension, ".pea") ||
           ends_with(extension, ".xpea") ||
           ends_with(extension, ".peac");
}

RenewalResult renew_manually(const char* certificate_id, uint64_t now) {
    RenewalResult result{};
    if (!certificate_id || !*certificate_id || now == 0) return result;

    // The actual protected signing service will replace this policy-only
    // implementation once user mode, persistent storage and crypto are online.
    result.success = true;
    result.applications_bound = 0;
    result.expires_at = now + kValiditySeconds;
    return result;
}

}
