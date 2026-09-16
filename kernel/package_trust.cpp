#include "package_trust.hpp"

namespace package_trust {

void init() {}

Result verify(const Package& package, uint64_t now) {
    if (package.kind == PackageKind::Foreign) return Result::Rejected;
    if (!package.digest || package.digest_size == 0) return Result::Rejected;
    if (!package.certificate) return Result::Rejected;

    const certificate::Status status = certificate::validate(package.certificate, now);
    if (status != certificate::Status::Valid) return Result::Rejected;

    // Cryptographic signature and digest binding remain deliberately fail-closed
    // until the trusted crypto implementation is linked into the kernel.
    return Result::Rejected;
}

}
