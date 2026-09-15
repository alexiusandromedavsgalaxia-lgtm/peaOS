#include "x90_features.hpp"

namespace x90_features {
namespace {
MorphState morph = {"Default", "balanced"};
Snapshot snapshot = {0, 0};
AppRuntime runtime = {RuntimeKind::Native, "peaOS native", true, false};
}

void init() {
    morph = {"Default", "balanced"};
    snapshot = {0, 0};
    runtime = {RuntimeKind::Native, "peaOS native", true, false};
}

const MorphState& morph_state() { return morph; }

bool set_morph(const char* name, const char* mode) {
    if (!name || !*name || !mode || !*mode) return false;
    morph.name = name;
    morph.mode = mode;
    return true;
}

const Snapshot& filesystem_snapshot() { return snapshot; }

bool create_snapshot(uint64_t id, uint64_t timestamp) {
    if (id == 0 || timestamp == 0) return false;
    snapshot = {id, timestamp};
    return true;
}

const AppRuntime& fusion_runtime() { return runtime; }

bool select_runtime(RuntimeKind kind, const char* name) {
    if (!name || !*name) return false;
    runtime.kind = kind;
    runtime.name = name;
    runtime.sandboxed = true;
    runtime.running = false;
    return true;
}
}
