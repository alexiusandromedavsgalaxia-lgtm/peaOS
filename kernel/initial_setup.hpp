#pragma once
#include <stdint.h>

namespace initial_setup {

enum class Stage : uint8_t {
    Language,
    Region,
    Network,
    Hardware,
    Activation,
    Security,
    Complete
};

void init();
Stage stage();
const char* title();
const char* status_line();
bool completed();
void advance();
void fail(const char* reason);
const char* failure_reason();

}
