#pragma once

namespace activation_screen {

enum class Page {
    Welcome,
    HardwareCheck,
    NetworkCheck,
    LicenseCheck,
    Activate,
    Complete,
    Failure
};

void init();
Page page();
const char* title();
const char* status_line();
bool can_continue();

}
