#pragma once

namespace x90_features {

// X90 Morph: desktop state/context model.
struct MorphState {
    const char* name;
    const char* mode;
};

// TimeMachine FS: versioned filesystem boundary.
struct Snapshot {
    unsigned long long id;
    unsigned long long timestamp;
};

// X90 Fusion: unified application/runtime boundary.
enum class RuntimeKind {
    Native,
    WindowsPE,
    AndroidAPK,
    MacOSMachO
};

struct AppRuntime {
    RuntimeKind kind;
    const char* name;
};

void init();
const MorphState& morph_state();
const Snapshot& filesystem_snapshot();
const AppRuntime& fusion_runtime();

}
