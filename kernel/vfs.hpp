#pragma once
#include <stdint.h>

namespace vfs {

enum class NodeType : uint8_t { File, Directory, Device };

struct Node {
    uint32_t id;
    NodeType type;
    const char* name;
    uint64_t size;
    bool readable;
    bool writable;
};

constexpr uint32_t kMaxNodes = 256;

void init();
const Node* root();
const Node* create_file(const char* name, uint64_t size);
const Node* create_directory(const char* name);
const Node* find(const char* name);
uint32_t count();

}
