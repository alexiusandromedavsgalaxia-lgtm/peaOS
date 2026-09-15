#pragma once
#include <stdint.h>
#include "../apps/system/app_registry.hpp"

namespace app_reader {
struct Entry { const char* name; app_registry::Runtime runtime; bool native; bool supported; };
struct Manifest { const char* name; const char* version; const char* package; const char* runtime; const char* entry; bool network; bool isolated; };
constexpr uint32_t kMaxEntries = 64;
constexpr uint32_t kMaxManifestText = 4096;
void init();
uint32_t parse(const char* manifest, uint32_t length);
bool read_manifest(const char* manifest, uint32_t length, Manifest* out);
const Entry* entries(uint32_t* count);
const Entry* find(const char* name);
const char* extension_for(const char* path);
bool is_native_package(const char* path);
bool is_supported_package(const char* path);
bool validate_package(const char* path, const char* manifest, uint32_t length);
}
