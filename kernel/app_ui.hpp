#pragma once
#include <stdint.h>
#include "apps/system/app_registry.hpp"

namespace app_ui {

enum class View : uint8_t { Home, List, Grid, Editor, Settings, Monitor, Browser, Player, Calendar, Clock, Network, Bluetooth, Security, Development, About };

enum class EventType : uint8_t { None, PointerDown, PointerUp, KeyDown, KeyUp, Text, Back, Submit, Close };

struct Rect { int32_t x; int32_t y; uint32_t width; uint32_t height; };
struct Event { EventType type; int32_t x; int32_t y; uint32_t key; const char* text; };
struct AppSurface { app_registry::AppId id; const char* title; const char* subtitle; View view; Rect bounds; bool focused; bool dirty; };

void init(uint32_t width, uint32_t height);
const AppSurface* surface(app_registry::AppId id);
bool open(app_registry::AppId id);
bool close(app_registry::AppId id);
bool focus(app_registry::AppId id);
bool dispatch(app_registry::AppId id, const Event& event);
void tick(uint64_t ticks);
uint32_t open_count();
const AppSurface* surfaces(uint32_t* count);
const char* view_name(View view);

}
