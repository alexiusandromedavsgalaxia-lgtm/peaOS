#pragma once
#include <stdint.h>
#include "app_registry.hpp"

namespace system_apps {
struct AppFrame { app_registry::AppId id; const char* title; const char* primary_action; const char* status; uint32_t item_count; bool network; bool privileged; };

AppFrame settings(); AppFrame files(); AppFrame software_center(); AppFrame terminal(); AppFrame task_manager(); AppFrame system_monitor();
AppFrame text_editor(); AppFrame media_player(); AppFrame photos(); AppFrame image_viewer(); AppFrame viewer(); AppFrame browser();
AppFrame notes(); AppFrame calendar(); AppFrame clock(); AppFrame network_manager(); AppFrame bluetooth(); AppFrame audio(); AppFrame display();
AppFrame users_accounts(); AppFrame security_center(); AppFrame permissions(); AppFrame notifications(); AppFrame about(); AppFrame help();
AppFrame pea_cloud(); AppFrame manga_reader(); AppFrame development(); AppFrame visual_studio_code(); AppFrame sandbox_interface();
AppFrame x90_fusion_manager(); AppFrame time_machine_manager(); AppFrame morph_manager(); AppFrame certificate_your_app(); AppFrame web_distribution();

const AppFrame* frame(app_registry::AppId id);
}
