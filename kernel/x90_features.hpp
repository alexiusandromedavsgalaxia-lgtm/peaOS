#pragma once

#include <stdint.h>

namespace x90_features {

struct MorphState {
    const char* name;
    const char* mode;
};

struct Snapshot {
    uint64_t id;
    uint64_t timestamp;
};

enum class RuntimeKind : uint8_t {
    Native,
    WindowsPE,
    AndroidAPK,
    MacOSMachO
};

struct AppRuntime {
    RuntimeKind kind;
    const char* name;
    bool sandboxed;
    bool running;
};

void init();

const MorphState& morph_state();
bool set_morph(const char* name, const char* mode);

const Snapshot& filesystem_snapshot();
bool create_snapshot(uint64_t id, uint64_t timestamp);

const AppRuntime& fusion_runtime();
bool select_runtime(RuntimeKind kind, const char* name);

}
