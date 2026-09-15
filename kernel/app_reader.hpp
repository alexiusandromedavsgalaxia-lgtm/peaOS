#pragma once
#include <stdint.h>
#include "../apps/system/app_registry.hpp"

namespace app_reader {
struct Entry { const char* name; app_registry::Runtime runtime; bool native; bool supported; };
constexpr uint32_t kMaxEntries = 64;
void init();
uint32_t parse(const char* manifest, uint32_t length);
const Entry* entries(uint32_t* count);
const Entry* find(const char* name);
const char* extension_for(const char* path);
bool is_native_package(const char* path);
bool is_supported_package(const char* path);
}
