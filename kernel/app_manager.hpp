#pragma once
#include <stdint.h>
#include "../apps/system/app_registry.hpp"

namespace app_manager {

struct RunningApp {
    app_registry::AppId id;
    uint32_t window_id;
    bool running;
};

void init();
bool launch(app_registry::AppId id);
bool close(app_registry::AppId id);
const RunningApp* running(uint32_t* count);
uint32_t running_count();
const char* last_status();

}
