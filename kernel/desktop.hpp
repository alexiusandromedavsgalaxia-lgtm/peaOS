#pragma once
#include <stdint.h>

namespace desktop {

enum class App : uint8_t {
    Settings,
    Photos,
    Viewer,
    Browser,
    Notes,
    PeaCloud,
    MangaReader,
    Development,
    VisualStudioCode,
    SandboxInterface
};

struct Window {
    uint32_t id;
    App app;
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
    bool visible;
    bool focused;
};

constexpr uint32_t kMaxWindows = 32;

void init(uint32_t width, uint32_t height);
uint32_t open(App app);
bool close(uint32_t id);
bool focus(uint32_t id);
const Window* windows(uint32_t* count);
uint32_t screen_width();
uint32_t screen_height();

}
