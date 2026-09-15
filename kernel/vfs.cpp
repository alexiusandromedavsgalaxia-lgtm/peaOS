#include "vfs.hpp"

namespace vfs {
namespace {
Node g_nodes[kMaxNodes]{};
uint32_t g_count = 0;
uint32_t g_next_id = 1;
char g_names[kMaxNodes][64]{};

bool copy_name(char* dst, const char* src) {
    if (!dst || !src || !*src) return false;
    uint32_t i = 0;
    for (; src[i] && i < 63; ++i) dst[i] = src[i];
    dst[i] = 0;
    return src[i] == 0;
}

bool same(const char* a, const char* b) {
    if (!a || !b) return false;
    while (*a && *b && *a == *b) { ++a; ++b; }
    return *a == *b;
}

bool name_exists(const char* name) {
    return find(name) != nullptr;
}
}

void init() {
    for (uint32_t i = 0; i < kMaxNodes; ++i) {
        g_nodes[i] = {};
        g_names[i][0] = 0;
    }
    g_count = 0;
    g_next_id = 1;
    create_directory("/");
}

const Node* root() { return g_count ? &g_nodes[0] : nullptr; }

const Node* create_file(const char* name, uint64_t size) {
    if (!name || g_count >= kMaxNodes || name_exists(name) || !copy_name(g_names[g_count], name)) return nullptr;
    g_nodes[g_count] = {g_next_id++, NodeType::File, g_names[g_count], size, true, true};
    return &g_nodes[g_count++];
}

const Node* create_directory(const char* name) {
    if (!name || g_count >= kMaxNodes || name_exists(name) || !copy_name(g_names[g_count], name)) return nullptr;
    g_nodes[g_count] = {g_next_id++, NodeType::Directory, g_names[g_count], 0, true, true};
    return &g_nodes[g_count++];
}

const Node* find(const char* name) {
    if (!name) return nullptr;
    for (uint32_t i = 0; i < g_count; ++i)
        if (same(g_nodes[i].name, name)) return &g_nodes[i];
    return nullptr;
}

uint32_t count() { return g_count; }

}
