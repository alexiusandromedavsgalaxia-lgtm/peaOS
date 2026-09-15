#include "initial_setup.hpp"

namespace initial_setup {
namespace {
Stage g_stage = Stage::Language;
const char* g_failure = "";
}

void init() {
    g_stage = Stage::Language;
    g_failure = "";
}

Stage stage() { return g_stage; }

const char* title() {
    switch (g_stage) {
        case Stage::Language: return "Idioma";
        case Stage::Region: return "Región";
        case Stage::Network: return "Conexión de red";
        case Stage::Hardware: return "Comprobación de hardware";
        case Stage::Activation: return "Activación de peaOS";
        case Stage::Security: return "Seguridad";
        case Stage::Complete: return "Configuración completada";
    }
    return "Configuración inicial";
}

const char* status_line() {
    if (g_failure[0] != '\0') return g_failure;
    switch (g_stage) {
        case Stage::Language: return "Selecciona el idioma del sistema.";
        case Stage::Region: return "Configura región, fecha y formato.";
        case Stage::Network: return "Configura una conexión para los servicios del sistema.";
        case Stage::Hardware: return "Verificando CPU, memoria, almacenamiento, firmware y gráficos.";
        case Stage::Activation: return "Verificando la licencia y la identidad de esta instalación.";
        case Stage::Security: return "Configura la protección y la cuenta del dispositivo.";
        case Stage::Complete: return "peaOS está listo para iniciar el escritorio.";
    }
    return "";
}

bool completed() { return g_stage == Stage::Complete && g_failure[0] == '\0'; }

void advance() {
    if (g_failure[0] != '\0' || completed()) return;
    switch (g_stage) {
        case Stage::Language: g_stage = Stage::Region; break;
        case Stage::Region: g_stage = Stage::Network; break;
        case Stage::Network: g_stage = Stage::Hardware; break;
        case Stage::Hardware: g_stage = Stage::Activation; break;
        case Stage::Activation: g_stage = Stage::Security; break;
        case Stage::Security: g_stage = Stage::Complete; break;
        case Stage::Complete: break;
    }
}

void fail(const char* reason) { g_failure = reason ? reason : "Error de configuración inicial."; }
const char* failure_reason() { return g_failure; }
}
