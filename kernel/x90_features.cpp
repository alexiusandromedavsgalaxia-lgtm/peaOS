#include "x90_features.hpp"

namespace x90_features {
static MorphState morph = {"Default", "balanced"};
static Snapshot snapshot = {0, 0};
static AppRuntime runtime = {RuntimeKind::Native, "peaOS native"};

void init() {
    morph.name = "Default";
    morph.mode = "balanced";
    snapshot.id = 0;
    snapshot.timestamp = 0;
    runtime.kind = RuntimeKind::Native;
    runtime.name = "peaOS native";
}

const MorphState& morph_state() { return morph; }
const Snapshot& filesystem_snapshot() { return snapshot; }
const AppRuntime& fusion_runtime() { return runtime; }
}
