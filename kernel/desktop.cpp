#include "desktop.hpp"

namespace desktop {
namespace {
Window g_windows[kMaxWindows]{};
uint32_t g_count = 0;
uint32_t g_next_id = 1;
uint32_t g_width = 1024;
uint32_t g_height = 768;

void clear_focus() {
    for (uint32_t i = 0; i < kMaxWindows; ++i)
        if (g_windows[i].visible) g_windows[i].focused = false;
}

void clamp_window(Window& w) {
    const uint32_t min_width = 240;
    const uint32_t min_height = 160;
    if (w.width < min_width) w.width = min_width;
    if (w.height < min_height) w.height = min_height;
    if (w.width > g_width) w.width = g_width;
    if (w.height > g_height) w.height = g_height;
    if (w.x < 0) w.x = 0;
    if (w.y < 0) w.y = 0;
    const int32_t max_x = static_cast<int32_t>(g_width - w.width);
    const int32_t max_y = static_cast<int32_t>(g_height - w.height);
    if (w.x > max_x) w.x = max_x;
    if (w.y > max_y) w.y = max_y;
}
}

void init(uint32_t width, uint32_t height) {
    g_width = width ? width : 1024;
    g_height = height ? height : 768;
    for (uint32_t i = 0; i < kMaxWindows; ++i) g_windows[i] = {};
    g_count = 0;
    g_next_id = 1;
}

uint32_t open(App app) {
    for (uint32_t i = 0; i < kMaxWindows; ++i) {
        if (!g_windows[i].visible) {
            const uint32_t id = g_next_id++;
            const uint32_t offset = (g_count % 8) * 28;
            Window w{id, app, static_cast<int32_t>(32 + offset), static_cast<int32_t>(32 + offset), 640, 480, true, false};
            clamp_window(w);
            clear_focus();
            w.focused = true;
            g_windows[i] = w;
            ++g_count;
            return id;
        }
    }
    return 0;
}

bool close(uint32_t id) {
    for (uint32_t i = 0; i < kMaxWindows; ++i) {
        if (g_windows[i].visible && g_windows[i].id == id) {
            const bool was_focused = g_windows[i].focused;
            g_windows[i] = {};
            if (g_count) --g_count;
            if (was_focused) {
                for (uint32_t j = 0; j < kMaxWindows; ++j) {
                    if (g_windows[j].visible) {
                        clear_focus();
                        g_windows[j].focused = true;
                        break;
                    }
                }
            }
            return true;
        }
    }
    return false;
}

bool focus(uint32_t id) {
    for (uint32_t i = 0; i < kMaxWindows; ++i) {
        if (g_windows[i].visible && g_windows[i].id == id) {
            clear_focus();
            g_windows[i].focused = true;
            return true;
        }
    }
    return false;
}

const Window* windows(uint32_t* count) {
    if (count) *count = g_count;
    return g_windows;
}

uint32_t screen_width() { return g_width; }
uint32_t screen_height() { return g_height; }

}
