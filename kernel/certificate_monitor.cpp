#include "certificate_monitor.hpp"

namespace certificate_monitor {
namespace {
CertificateRecord g_records[kMaxTrackedCertificates]{};
Inventory g_inventory{};
uint64_t g_last_check = 0;

void clear_inventory() { g_inventory = {}; }

void add_state(InstallState state) {
    switch (state) {
        case InstallState::Active: ++g_inventory.active; break;
        case InstallState::Revoked: ++g_inventory.revoked; break;
        case InstallState::Expired: ++g_inventory.expired; break;
        case InstallState::Invalid: ++g_inventory.invalid; break;
        case InstallState::ServerUnavailable: ++g_inventory.server_unavailable; break;
        default: break;
    }
}

int find_index(uint64_t serial) {
    if (!serial) return -1;
    for (uint32_t i = 0; i < kMaxTrackedCertificates; ++i)
        if (g_records[i].serial == serial) return static_cast<int>(i);
    return -1;
}

int free_index() {
    for (uint32_t i = 0; i < kMaxTrackedCertificates; ++i)
        if (g_records[i].serial == 0) return static_cast<int>(i);
    return -1;
}

void recompute_inventory() {
    clear_inventory();
    for (uint32_t i = 0; i < kMaxTrackedCertificates; ++i) {
        const CertificateRecord& r = g_records[i];
        if (!r.serial) continue;
        ++g_inventory.total;
        if (r.installed) ++g_inventory.installed;
        if (r.distribution == static_cast<uint8_t>(certificate::Distribution::Local))
            ++g_inventory.local_distribution;
        else if (r.distribution == static_cast<uint8_t>(certificate::Distribution::WebDistribution))
            ++g_inventory.web_distribution;
        add_state(r.state);
    }
}

InstallState state_from_server(const certificate::Certificate* cert,
                               const certificate::ServerValidation& server,
                               const uint8_t* expected_origin_hash,
                               const uint8_t* package_hash,
                               uint64_t now) {
    const certificate::Status status = certificate::validate_server(
        cert, server, expected_origin_hash, package_hash);
    if (status == certificate::Status::Revoked || status == certificate::Status::NotActiveOnServer)
        return InstallState::Revoked;
    if (status == certificate::Status::ServerUnavailable)
        return InstallState::ServerUnavailable;
    if (status == certificate::Status::Expired)
        return InstallState::Expired;
    if (status != certificate::Status::Valid)
        return InstallState::Invalid;
    if (certificate::expired(cert, now))
        return InstallState::Expired;
    return InstallState::Active;
}

}

void init(uint64_t now) {
    for (uint32_t i = 0; i < kMaxTrackedCertificates; ++i) g_records[i] = {};
    g_last_check = now;
    clear_inventory();
}

bool register_certificate(const certificate::Certificate* cert, bool installed, uint64_t now) {
    if (!cert || cert->magic != certificate::kMagic || cert->version != certificate::kVersion || cert->serial == 0)
        return false;
    int index = find_index(cert->serial);
    if (index < 0) index = free_index();
    if (index < 0) return false;

    CertificateRecord& r = g_records[index];
    r.serial = cert->serial;
    for (uint32_t i = 0; i < sizeof(r.certificate_id); ++i) r.certificate_id[i] = cert->certificate_id[i];
    r.distribution = cert->distribution;
    r.installed = installed;
    r.server_active = false;
    r.server_revoked = false;
    r.state = certificate::expired(cert, now) ? InstallState::Expired : InstallState::Unknown;
    r.last_checked = now;
    recompute_inventory();
    return true;
}

bool remove_certificate(uint64_t serial) {
    const int index = find_index(serial);
    if (index < 0) return false;
    g_records[index] = {};
    recompute_inventory();
    return true;
}

bool check_certificate(uint64_t serial, const certificate::ServerValidation& server,
                       const uint8_t* expected_origin_hash, const uint8_t* package_hash,
                       uint64_t now) {
    const int index = find_index(serial);
    if (index < 0) return false;

    CertificateRecord& r = g_records[index];
    certificate::Certificate cert{};
    cert.magic = certificate::kMagic;
    cert.version = certificate::kVersion;
    cert.serial = r.serial;
    cert.distribution = r.distribution;
    for (uint32_t i = 0; i < sizeof(cert.certificate_id); ++i) cert.certificate_id[i] = r.certificate_id[i];

    r.server_active = server.active;
    r.server_revoked = server.revoked;
    r.state = state_from_server(&cert, server, expected_origin_hash, package_hash, now);
    r.last_checked = now;
    recompute_inventory();
    return true;
}

uint32_t check_all(const certificate::ServerValidation& server,
                   const uint8_t* expected_origin_hash, const uint8_t* package_hash,
                   uint64_t now) {
    uint32_t checked = 0;
    for (uint32_t i = 0; i < kMaxTrackedCertificates; ++i) {
        if (!g_records[i].serial) continue;
        if (check_certificate(g_records[i].serial, server, expected_origin_hash, package_hash, now))
            ++checked;
    }
    g_last_check = now;
    return checked;
}

bool tick(uint64_t now, const certificate::ServerValidation& server,
          const uint8_t* expected_origin_hash, const uint8_t* package_hash) {
    if (now < g_last_check || now - g_last_check < kCheckIntervalSeconds) return false;
    check_all(server, expected_origin_hash, package_hash, now);
    return true;
}

const Inventory& inventory() { return g_inventory; }
const CertificateRecord* records(uint32_t* count) {
    if (count) *count = kMaxTrackedCertificates;
    return g_records;
}

}
