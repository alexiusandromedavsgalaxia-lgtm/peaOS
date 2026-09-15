#pragma once

namespace kernel_version {

// Version progression: each release increments by 100.50.30.
// Previous: v0.0.782.94.55
// Current:  v0.0.882.144.85
constexpr unsigned kMajor = 0;
constexpr unsigned kMinor = 0;
constexpr unsigned kPatch = 882;
constexpr unsigned kBuild = 144;
constexpr unsigned kRevision = 85;

constexpr const char* kFullVersion = "v0.0.882.144.85";

}
