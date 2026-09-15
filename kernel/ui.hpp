#pragma once
#include <stdint.h>

namespace ui {

struct Theme {
    uint32_t background;
    uint32_t surface;
    uint32_t surface_alt;
    uint32_t accent;
    uint32_t text;
    uint32_t muted;
    uint8_t corner_radius;
    uint8_t spacing;
};

void init(uint32_t width, uint32_t height);
const Theme& theme();
uint32_t width();
uint32_t height();
const char* style_name();

}
