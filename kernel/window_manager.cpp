#include "window_manager.hpp"

namespace window_manager {
namespace {
WindowInfo g_windows[kMaxWindows]{};
uint32_t g_count = 0;
uint32_t g_next_id = 1;
uint32_t g_next_z = 1;
uint32_t g_width = 1024;
uint32_t g_height = 768;
uint32_t g_focused = 0;

WindowInfo* find(uint32_t id) {
    for (uint32_t i = 0; i < kMaxWindows; ++i)
        if (g_windows[i].visible && g_windows[i].id == id) return &g_windows[i];
    return nullptr;
}

void clamp(WindowInfo& w) {
    if (w.width < 280) w.width = 280;
    if (w.height < 180) w.height = 180;
    if (w.width > g_width) w.width = g_width;
    if (w.height > g_height) w.height = g_height;
    const int32_t mx = static_cast<int32_t>(g_width - w.width);
    const int32_t my = static_cast<int32_t>(g_height - w.height);
    if (w.x < 0) w.x = 0;
    if (w.y < 0) w.y = 0;
    if (w.x > mx) w.x = mx;
    if (w.y > my) w.y = my;
}

void clear_focus() {
    for (auto& w : g_windows) w.focused = false;
    g_focused = 0;
}

void raise(WindowInfo& w) {
    w.z = g_next_z++;
    clear_focus();
    w.focused = true;
    g_focused = w.id;
}

void save_restore_geometry(WindowInfo& w) {
    if (w.state == State::Normal) {
        w.restore_x = w.x;
        w.restore_y = w.y;
        w.restore_width = w.width;
        w.restore_height = w.height;
    }
}

void restore_geometry(WindowInfo& w) {
    w.x = w.restore_x;
    w.y = w.restore_y;
    w.width = w.restore_width;
    w.height = w.restore_height;
    clamp(w);
}

void focus_topmost() {
    WindowInfo* best = nullptr;
    for (auto& w : g_windows) {
        if (!w.visible || w.state == State::Minimized) continue;
        if (!best || w.z > best->z) best = &w;
    }
    if (best) raise(*best);
    else clear_focus();
}
}

void init(uint32_t width, uint32_t height) {
    g_width = width ? width : 1024;
    g_height = height ? height : 768;
    for (auto& w : g_windows) w = {};
    g_count = 0;
    g_next_id = 1;
    g_next_z = 1;
    g_focused = 0;
}

uint32_t open(desktop::App app, uint32_t width, uint32_t height) {
    for (uint32_t i = 0; i < kMaxWindows; ++i) {
        if (g_windows[i].visible) continue;
        WindowInfo w{};
        w.id = g_next_id++;
        w.app = app;
        w.x = 40 + static_cast<int32_t>((g_count % 7) * 28);
        w.y = 40 + static_cast<int32_t>((g_count % 7) * 28);
        w.width = width ? width : 720;
        w.height = height ? height : 480;
        w.state = State::Normal;
        w.snap = Snap::None;
        w.visible = true;
        clamp(w);
        w.restore_x = w.x;
        w.restore_y = w.y;
        w.restore_width = w.width;
        w.restore_height = w.height;
        g_windows[i] = w;
        ++g_count;
        raise(g_windows[i]);
        return w.id;
    }
    return 0;
}

bool close(uint32_t id) {
    WindowInfo* w = find(id);
    if (!w) return false;
    const bool was_focused = g_focused == id;
    *w = {};
    if (g_count) --g_count;
    if (was_focused) focus_topmost();
    return true;
}

bool focus(uint32_t id) {
    WindowInfo* w = find(id);
    if (!w) return false;
    if (w->state == State::Minimized) {
        w->state = State::Normal;
        restore_geometry(*w);
        w->snap = Snap::None;
    }
    raise(*w);
    return true;
}

bool move(uint32_t id, int32_t x, int32_t y) {
    WindowInfo* w = find(id);
    if (!w || w->state == State::Minimized || w->state == State::Fullscreen) return false;
    if (w->state == State::Maximized) restore(id), w = find(id);
    if (!w) return false;
    save_restore_geometry(*w);
    w->x = x;
    w->y = y;
    w->snap = Snap::None;
    clamp(*w);
    raise(*w);
    return true;
}

bool resize(uint32_t id, uint32_t width, uint32_t height) {
    WindowInfo* w = find(id);
    if (!w || w->state == State::Minimized || w->state == State::Fullscreen) return false;
    if (w->state == State::Maximized) restore(id), w = find(id);
    if (!w) return false;
    w->state = State::Normal;
    w->snap = Snap::None;
    w->width = width ? width : 280;
    w->height = height ? height : 180;
    clamp(*w);
    save_restore_geometry(*w);
    raise(*w);
    return true;
}

bool minimize(uint32_t id) {
    WindowInfo* w = find(id);
    if (!w) return false;
    if (w->state == State::Normal) save_restore_geometry(*w);
    w->state = State::Minimized;
    w->focused = false;
    if (g_focused == id) {
        g_focused = 0;
        focus_topmost();
    }
    return true;
}

bool maximize(uint32_t id) {
    WindowInfo* w = find(id);
    if (!w || w->state == State::Minimized) return false;
    if (w->state == State::Normal) save_restore_geometry(*w);
    w->state = State::Maximized;
    w->snap = Snap::None;
    w->x = 0;
    w->y = 0;
    w->width = g_width;
    w->height = g_height;
    raise(*w);
    return true;
}

bool restore(uint32_t id) {
    WindowInfo* w = find(id);
    if (!w) return false;
    if (w->state == State::Minimized) {
        w->state = State::Normal;
        restore_geometry(*w);
    } else if (w->state == State::Maximized || w->state == State::Fullscreen) {
        w->state = State::Normal;
        restore_geometry(*w);
    } else if (w->snap != Snap::None) {
        w->snap = Snap::None;
        restore_geometry(*w);
    }
    w->snap = Snap::None;
    raise(*w);
    return true;
}

bool fullscreen(uint32_t id) {
    WindowInfo* w = find(id);
    if (!w || w->state == State::Minimized) return false;
    if (w->state == State::Normal || w->snap != Snap::None) save_restore_geometry(*w);
    w->state = State::Fullscreen;
    w->snap = Snap::None;
    w->x = 0;
    w->y = 0;
    w->width = g_width;
    w->height = g_height;
    raise(*w);
    return true;
}

bool snap(uint32_t id, Snap side) {
    WindowInfo* w = find(id);
    if (!w || w->state == State::Minimized || w->state == State::Fullscreen) return false;
    if (side == Snap::None) return restore(id);
    if (w->state == State::Normal && w->snap == Snap::None) save_restore_geometry(*w);
    w->state = State::Normal;
    w->snap = side;
    if (side == Snap::Left) {
        w->x = 0; w->y = 0; w->width = g_width / 2; w->height = g_height;
    } else if (side == Snap::Right) {
        w->x = static_cast<int32_t>(g_width / 2); w->y = 0; w->width = g_width - g_width / 2; w->height = g_height;
    } else if (side == Snap::Top) {
        w->x = 0; w->y = 0; w->width = g_width; w->height = g_height / 2;
    } else {
        w->x = 0; w->y = static_cast<int32_t>(g_height / 2); w->width = g_width; w->height = g_height - g_height / 2;
    }
    raise(*w);
    return true;
}

Hit hit_test(uint32_t id, int32_t x, int32_t y) {
    WindowInfo* w = find(id);
    if (!w || w->state == State::Minimized) return Hit::None;
    const int32_t rx = x - w->x;
    const int32_t ry = y - w->y;
    if (rx < 0 || ry < 0 || rx >= static_cast<int32_t>(w->width) || ry >= static_cast<int32_t>(w->height)) return Hit::None;
    if (w->state == State::Fullscreen) return Hit::Content;
    if (ry < 42) {
        if (rx >= static_cast<int32_t>(w->width) - 42) return Hit::Close;
        if (rx >= static_cast<int32_t>(w->width) - 84) return Hit::Maximize;
        if (rx >= static_cast<int32_t>(w->width) - 126) return Hit::Minimize;
        return Hit::Titlebar;
    }
    const bool l = rx < 8;
    const bool r = rx >= static_cast<int32_t>(w->width) - 8;
    const bool t = ry < 8;
    const bool b = ry >= static_cast<int32_t>(w->height) - 8;
    if (l) return Hit::ResizeLeft;
    if (r) return Hit::ResizeRight;
    if (t) return Hit::ResizeTop;
    if (b) return Hit::ResizeBottom;
    return Hit::Content;
}

const WindowInfo* windows(uint32_t* count) { if (count) *count = g_count; return g_windows; }
uint32_t focused() { return g_focused; }
uint32_t screen_width() { return g_width; }
uint32_t screen_height() { return g_height; }
}
