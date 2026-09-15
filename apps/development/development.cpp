#include "development.hpp"

namespace development {
namespace {
MacLink g_mac{MacState::RequiresMac, false, false, 0};
const char* g_status = "Selecciona un objetivo de desarrollo.";
bool g_toolchain_ready[4] = {false, false, false, false};
}

void init() {
    g_mac = {MacState::RequiresMac, false, false, 0};
    g_status = "Selecciona un objetivo de desarrollo.";
    for (bool& ready : g_toolchain_ready) ready = false;
}

Project create_project(Target target, const char* name) {
    Project p{target, name, target_extension(target), target == Target::MacOS};
    if (!name || !*name) {
        g_status = "No se puede crear un proyecto sin nombre.";
    } else if (!target_available(target)) {
        g_status = "El toolchain seleccionado no está disponible.";
    } else {
        g_status = "Proyecto creado.";
    }
    return p;
}

const char* target_name(Target target) {
    switch (target) {
        case Target::PeaOS: return "peaOS";
        case Target::Windows: return "Windows";
        case Target::Android: return "Android";
        case Target::MacOS: return "macOS";
    }
    return "Desconocido";
}

const char* target_extension(Target target) {
    switch (target) {
        case Target::PeaOS: return ".pea/.xpea/.peac";
        case Target::Windows: return ".exe/.dll";
        case Target::Android: return ".apk/.aab";
        case Target::MacOS: return ".app";
    }
    return "";
}

bool target_available(Target target) {
    if (target == Target::MacOS)
        return g_mac.usb_c_connected && g_mac.xcode_open && g_mac.state == MacState::Ready;
    return g_toolchain_ready[static_cast<uint8_t>(target)];
}

void set_mac_connection(bool connected, bool xcode_open) {
    g_mac.usb_c_connected = connected;
    g_mac.xcode_open = xcode_open;
    g_mac.synced_files = 0;
    if (!connected) {
        g_mac.state = MacState::Disconnected;
        g_status = "Mac desconectado: las operaciones de macOS se han detenido.";
    } else if (!xcode_open) {
        g_mac.state = MacState::WaitingForXcode;
        g_status = "Abre Xcode en el Mac conectado para continuar.";
    } else {
        g_mac.state = MacState::Ready;
        g_status = "Mac y Xcode listos. Sincronización por USB-C disponible.";
    }
}

MacLink mac_link() { return g_mac; }

bool request_build(Target target) {
    if (!target_available(target)) {
        g_status = "No se puede compilar: el toolchain/entorno requerido no está disponible.";
        return false;
    }
    if (target == Target::MacOS) {
        g_mac.state = MacState::BuildRunning;
        g_status = "Orden de compilación enviada a Xcode por USB-C.";
        return true;
    }
    g_status = "Orden de compilación aceptada por el toolchain del objetivo.";
    return true;
}

const char* status() { return g_status; }
}
