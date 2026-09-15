#include "app_reader.hpp"

namespace app_reader {
namespace {
Entry g_entries[kMaxEntries]{};
uint32_t g_count = 0;
char g_names[kMaxEntries][48]{};
char g_values[kMaxEntries][128]{};

bool eq(const char* a, const char* b) {
    if (!a || !b) return false;
    while (*a && *b && *a == *b) { ++a; ++b; }
    return *a == *b;
}

const char* trim_left(const char* p) {
    while (p && (*p == ' ' || *p == '\t')) ++p;
    return p;
}

void trim_copy(char* dst, const char* src, uint32_t capacity) {
    if (!dst || !capacity) return;
    uint32_t start = 0;
    while (src && (src[start] == ' ' || src[start] == '\t')) ++start;
    uint32_t end = start;
    while (src && src[end] && src[end] != '\r' && src[end] != '\n') ++end;
    while (end > start && (src[end - 1] == ' ' || src[end - 1] == '\t')) --end;
    uint32_t i = 0;
    while (start < end && i + 1 < capacity) dst[i++] = src[start++];
    dst[i] = 0;
}

bool key_value(const char* line, const char* key, char* out, uint32_t out_size) {
    if (!line || !key || !out || !out_size) return false;
    const char* p = trim_left(line);
    uint32_t key_len = 0;
    while (key[key_len]) ++key_len;
    uint32_t i = 0;
    while (i < key_len && p[i] == key[i]) ++i;
    if (i != key_len || p[i] != '=') return false;
    trim_copy(out, p + i + 1, out_size);
    return out[0] != 0;
}

bool next_line(const char* text, uint32_t length, uint32_t* pos, const char** line, uint32_t* line_length) {
    if (!text || !pos || !line || !line_length || *pos >= length) return false;
    const uint32_t start = *pos;
    uint32_t end = start;
    while (end < length && text[end] != '\n') ++end;
    *line = text + start;
    *line_length = end - start;
    *pos = end < length ? end + 1 : end;
    return true;
}

app_registry::Runtime runtime_from(const char* s) {
    if (eq(s, "x90") || eq(s, "fusion") || eq(s, "x90-fusion")) return app_registry::Runtime::X90Fusion;
    if (eq(s, "web") || eq(s, "web-sandbox")) return app_registry::Runtime::WebSandbox;
    return app_registry::Runtime::Native;
}

bool runtime_known(const char* s) {
    return eq(s, "native") || eq(s, "pea") || eq(s, "x90") || eq(s, "fusion") ||
           eq(s, "x90-fusion") || eq(s, "web") || eq(s, "web-sandbox");
}

bool truthy(const char* s) { return eq(s, "1") || eq(s, "true") || eq(s, "yes"); }

bool supported_key(const char* key) {
    return eq(key, "name") || eq(key, "version") || eq(key, "package") ||
           eq(key, "runtime") || eq(key, "entry") || eq(key, "network") || eq(key, "isolated");
}
}

void init() {
    for (auto& e : g_entries) e = {};
    for (auto& n : g_names) n[0] = 0;
    for (auto& v : g_values) v[0] = 0;
    g_count = 0;
}

uint32_t parse(const char* manifest, uint32_t length) {
    init();
    if (!manifest || !length || length > kMaxManifestText) return 0;
    uint32_t pos = 0;
    while (g_count < kMaxEntries) {
        const char* line = nullptr;
        uint32_t line_length = 0;
        if (!next_line(manifest, length, &pos, &line, &line_length)) break;
        if (!line_length) continue;
        const char* p = trim_left(line);
        if (!*p || *p == '#' || *p == ';' || *p == '[') continue;

        uint32_t eq_pos = 0;
        while (eq_pos < line_length && line[eq_pos] != '=') ++eq_pos;
        if (eq_pos == 0 || eq_pos >= line_length) continue;

        char key[48]{};
        char value[128]{};
        uint32_t key_copy = eq_pos < sizeof(key) ? eq_pos : sizeof(key) - 1;
        for (uint32_t i = 0; i < key_copy; ++i) key[i] = line[i];
        key[key_copy] = 0;
        trim_copy(g_names[g_count], key, sizeof(g_names[g_count]));
        if (!supported_key(g_names[g_count])) continue;
        trim_copy(value, line + eq_pos + 1, sizeof(value));
        if (!value[0]) continue;
        for (uint32_t i = 0; i + 1 < sizeof(g_values[g_count]) && value[i]; ++i) g_values[g_count][i] = value[i];
        g_values[g_count][sizeof(g_values[g_count]) - 1] = 0;
        g_entries[g_count] = {g_names[g_count], runtime_from(g_values[g_count]), is_native_package(g_values[g_count]), is_supported_package(g_values[g_count])};
        ++g_count;
    }
    return g_count;
}

bool read_manifest(const char* manifest, uint32_t length, Manifest* out) {
    if (!out || !manifest || !length || length > kMaxManifestText) return false;
    char name[128]{}, version[128]{}, package[128]{}, runtime[128]{}, entry[128]{}, network[128]{}, isolated[128]{};
    if (!value(manifest, length, "name", name, sizeof(name))) return false;
    if (!value(manifest, length, "package", package, sizeof(package))) return false;
    if (!value(manifest, length, "runtime", runtime, sizeof(runtime))) return false;
    if (!value(manifest, length, "entry", entry, sizeof(entry))) return false;
    if (!runtime_known(runtime)) return false;
    value(manifest, length, "version", version, sizeof(version));
    value(manifest, length, "network", network, sizeof(network));
    value(manifest, length, "isolated", isolated, sizeof(isolated));
    static Manifest m{};
    static char n[128], v[128], p[128], r[128], e[128];
    trim_copy(n, name, sizeof(n));
    trim_copy(v, version, sizeof(v));
    trim_copy(p, package, sizeof(p));
    trim_copy(r, runtime, sizeof(r));
    trim_copy(e, entry, sizeof(e));
    m = {n, v, p, r, e, truthy(network), truthy(isolated)};
    *out = m;
    return true;
}

const Entry* entries(uint32_t* count) { if (count) *count = g_count; return g_entries; }
const Entry* find(const char* name) {
    if (!name) return nullptr;
    for (uint32_t i = 0; i < g_count; ++i) if (eq(g_entries[i].name, name)) return &g_entries[i];
    return nullptr;
}

const char* extension_for(const char* path) {
    if (!path || !*path) return "";
    const char* slash = path;
    const char* dot = nullptr;
    for (const char* p = path; *p; ++p) {
        if (*p == '/' || *p == '\\') slash = p + 1;
        else if (*p == '.') dot = p;
    }
    return dot && dot >= slash && dot[1] ? dot : "";
}

bool is_native_package(const char* path) {
    const char* e = extension_for(path);
    return eq(e, ".pea") || eq(e, ".xpea") || eq(e, ".peac");
}

bool is_supported_package(const char* path) {
    const char* e = extension_for(path);
    return is_native_package(path) || eq(e, ".exe") || eq(e, ".dll") || eq(e, ".apk") || eq(e, ".aab");
}

bool validate_package(const char* path, const char* manifest, uint32_t length) {
    if (!path || !is_supported_package(path) || !manifest || !length) return false;
    Manifest m{};
    if (!read_manifest(manifest, length, &m)) return false;
    const char* path_ext = extension_for(path);
    const char* package_ext = extension_for(m.package);
    if (package_ext[0] && !eq(path_ext, package_ext)) return false;
    if (is_native_package(path) && runtime_from(m.runtime) != app_registry::Runtime::Native && runtime_from(m.runtime) != app_registry::Runtime::X90Fusion) return false;
    return true;
}

}
