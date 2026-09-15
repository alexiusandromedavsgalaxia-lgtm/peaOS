#include "development.hpp"

namespace development {
namespace {
MacLink g_mac{MacState::RequiresMac, false, false, 0};
AndroidWorkspace g_android{AndroidState::Unavailable, false, false, false};
WindowsLink g_windows{WindowsToolchain::Unavailable, false, false};
const char* g_status = "Selecciona un objetivo de desarrollo.";
bool g_toolchain_ready[4] = {true, false, false, false};
}

void init() {
    g_mac = {MacState::RequiresMac, false, false, 0};
    g_android = {AndroidState::LocalIsolatedReady, true, true, false};
    g_windows = {WindowsToolchain::Unavailable, false, false};
    g_status = "Selecciona un objetivo de desarrollo.";
    g_toolchain_ready[0] = true;
    g_toolchain_ready[1] = false;
    g_toolchain_ready[2] = true;
    g_toolchain_ready[3] = false;
}

Project create_project(Target target, const char* name) {
    const bool isolated = target == Target::Android;
    Project p{target, name, target_extension(target), target == Target::MacOS, isolated};
    if (!name || !*name) g_status = "No se puede crear un proyecto sin nombre.";
    else if (!target_available(target)) g_status = "El entorno/toolchain seleccionado no está disponible.";
    else if (target == Target::Android && (!g_android.virtualized || !g_android.visible || g_android.host_filesystem_shared)) g_status = "Android requiere un espacio virtual local aislado y visible.";
    else g_status = isolated ? "Proyecto Android creado en el espacio virtual local aislado." : "Proyecto creado.";
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
    if (target == Target::Windows)
        return g_windows.installed && g_windows.official;
    if (target == Target::Android)
        return g_android.virtualized && g_android.visible && !g_android.host_filesystem_shared;
    return g_toolchain_ready[static_cast<uint8_t>(target)];
}

void set_toolchain_ready(Target target, bool ready) {
    const uint8_t index = static_cast<uint8_t>(target);
    if (index >= 4 || target == Target::MacOS || target == Target::Android || target == Target::Windows) return;
    g_toolchain_ready[index] = ready;
    g_status = ready ? "Toolchain disponible." : "Toolchain marcado como no disponible.";
}

void set_windows_toolchain(WindowsToolchain toolchain, bool installed) {
    g_windows.toolchain = toolchain;
    g_windows.installed = installed;
    g_windows.official = installed && (toolchain == WindowsToolchain::MSVC || toolchain == WindowsToolchain::LLVMClang || toolchain == WindowsToolchain::MinGW);
    g_status = g_windows.official ? "Toolchain oficial/validado de Windows disponible." : "Toolchain de Windows no disponible.";
}

WindowsLink windows_toolchain() { return g_windows; }

void set_android_workspace(bool ready, bool visible) {
    g_android.virtualized = ready;
    g_android.visible = visible;
    g_android.host_filesystem_shared = false;
    g_android.state = ready && visible ? AndroidState::LocalIsolatedReady : AndroidState::Unavailable;
    g_status = ready && visible ? "Espacio Android local, virtualizado, aislado y visible listo." : "Espacio Android local no disponible.";
}

AndroidWorkspace android_workspace() { return g_android; }

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
    if (target == Target::Android) {
        g_android.state = AndroidState::BuildRunning;
        g_status = "Compilación Android ejecutándose dentro del espacio virtual local aislado.";
        return true;
    }
    if (target == Target::Windows) {
        g_status = "Compilación Windows delegada al toolchain oficial instalado.";
        return true;
    }
    if (target == Target::MacOS) {
        g_mac.state = MacState::BuildRunning;
        g_status = "Orden de compilación enviada a Xcode por USB-C.";
        return true;
    }
    g_status = "Orden de compilación aceptada por el toolchain nativo.";
    return true;
}

const char* status() { return g_status; }
}
