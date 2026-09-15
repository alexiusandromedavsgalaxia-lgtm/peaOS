#include "activation-screen.hpp"

namespace activation_screen {
namespace {
Page current = Page::Welcome;
}

void init() { current = Page::Welcome; }
Page page() { return current; }

const char* title() {
    switch (current) {
        case Page::Welcome: return "Activar peaOS";
        case Page::HardwareCheck: return "Comprobando hardware";
        case Page::NetworkCheck: return "Comprobando conexión";
        case Page::LicenseCheck: return "Verificando licencia";
        case Page::Activate: return "Activando peaOS";
        case Page::Complete: return "peaOS está activado";
        case Page::Failure: return "No se puede activar peaOS";
    }
    return "Activación";
}

const char* status_line() {
    switch (current) {
        case Page::Welcome: return "Vamos a comprobar el dispositivo antes de activar el sistema.";
        case Page::HardwareCheck: return "CPU, RAM, almacenamiento, firmware y gráficos.";
        case Page::NetworkCheck: return "Conectando con el servicio de activación de peaOS.";
        case Page::LicenseCheck: return "Comprobando origen, licencia y autenticidad.";
        case Page::Activate: return "Creando la identidad de esta instalación.";
        case Page::Complete: return "Hardware y licencia verificados correctamente.";
        case Page::Failure: return "Revisa los requisitos y vuelve a intentarlo.";
    }
    return "";
}

bool can_continue() {
    return current == Page::Welcome || current == Page::Complete;
}

}
