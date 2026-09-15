#include "app_security.hpp"

namespace app_security {

void init() {}

Decision authorize(const Request& request, bool user_explicitly_allowed) {
    // No application receives a capability merely because its SDK asks for it.
    // A missing package identity or explicit user grant always fails closed.
    if (!request.package_digest || request.package_digest_size == 0) return Decision::Deny;
    if (!user_explicitly_allowed) return Decision::Deny;
    return Decision::Allow;
}

}
