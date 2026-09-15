#pragma once
#include <stdint.h>
#include "desktop.hpp"

namespace window_manager {

enum class State : uint8_t { Normal, Minimized, Maximized, Fullscreen };
enum class Snap : uint8_t { None, Left, Right, Top, Bottom };

enum class Hit : uint8_t { None, Titlebar, Close, Minimize, Maximize, ResizeLeft, ResizeRight, ResizeTop, ResizeBottom, Content };

struct WindowInfo {
    uint32_t id;
    desktop::App app;
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t z;
    State state;
    Snap snap;
    bool visible;
    bool focused;
};

constexpr uint32_t kMaxWindows = 64;

void init(uint32_t width, uint32_t height);
uint32_t open(desktop::App app, uint32_t width = 720, uint32_t height = 480);
bool close(uint32_t id);
bool focus(uint32_t id);
bool move(uint32_t id, int32_t x, int32_t y);
bool resize(uint32_t id, uint32_t width, uint32_t height);
bool minimize(uint32_t id);
bool maximize(uint32_t id);
bool restore(uint32_t id);
bool fullscreen(uint32_t id);
bool snap(uint32_t id, Snap side);
Hit hit_test(uint32_t id, int32_t x, int32_t y);
const WindowInfo* windows(uint32_t* count);
uint32_t focused();
uint32_t screen_width();
uint32_t screen_height();

}
