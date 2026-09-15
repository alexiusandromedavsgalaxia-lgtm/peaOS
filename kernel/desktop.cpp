#include "desktop.hpp"

namespace desktop {
namespace {
Window g_windows[kMaxWindows]{};
uint32_t g_count = 0;
uint32_t g_next_id = 1;
uint32_t g_width = 80;
uint32_t g_height = 25;
}

void init(uint32_t width, uint32_t height) {
    for (uint32_t i = 0; i < kMaxWindows; ++i) g_windows[i] = {};
    g_count = 0;
    g_next_id = 1;
    g_width = width ? width : 80;
    g_height = height ? height : 25;
}

uint32_t open(App app) {
    for (uint32_t i = 0; i < kMaxWindows; ++i) {
        if (!g_windows[i].visible) {
            const uint32_t id = g_next_id++;
            const uint32_t offset = g_count * 24;
            g_windows[i] = {id, app, static_cast<int32_t>(offset % (g_width > 320 ? g_width - 320 : 1)), static_cast<int32_t>(offset % (g_height > 220 ? g_height - 220 : 1)), 640, 480, true, true};
            for (uint32_t j = 0; j < kMaxWindows; ++j)
                if (j != i) g_windows[j].focused = false;
            ++g_count;
            return id;
        }
    }
    return 0;
}

bool close(uint32_t id) {
    for (uint32_t i = 0; i < kMaxWindows; ++i) {
        if (g_windows[i].visible && g_windows[i].id == id) {
            g_windows[i].visible = false;
            g_windows[i].focused = false;
            if (g_count) --g_count;
            return true;
        }
    }
    return false;
}

bool focus(uint32_t id) {
    bool found = false;
    for (uint32_t i = 0; i < kMaxWindows; ++i) {
        if (g_windows[i].visible) g_windows[i].focused = false;
        if (g_windows[i].visible && g_windows[i].id == id) found = true;
    }
    if (found) {
        for (uint32_t i = 0; i < kMaxWindows; ++i)
            if (g_windows[i].visible && g_windows[i].id == id) g_windows[i].focused = true;
    }
    return found;
}

const Window* windows(uint32_t* count) {
    if (count) *count = g_count;
    return g_windows;
}

uint32_t screen_width() { return g_width; }
uint32_t screen_height() { return g_height; }

}
