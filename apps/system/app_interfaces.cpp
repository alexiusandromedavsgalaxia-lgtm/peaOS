#include "app_interfaces.hpp"

namespace system_apps {
namespace {
#define FRAME(id, title, action, status, items, net, priv) AppFrame{app_registry::AppId::id, title, action, status, items, net, priv}
const AppFrame kFrames[] = {
    FRAME(Settings,"Settings","Save","System preferences",8,false,false),
    FRAME(Files,"Files","Open","Filesystem browser",12,false,false),
    FRAME(SoftwareCenter,"Software Center","Install","Package catalog requires network",0,true,true),
    FRAME(Terminal,"Terminal","Run","Command shell",0,false,true),
    FRAME(TaskManager,"Task Manager","Terminate","Process control",0,false,true),
    FRAME(SystemMonitor,"System Monitor","Refresh","CPU, memory and process telemetry",0,false,true),
    FRAME(TextEditor,"Text Editor","Save","Text document",0,false,false),
    FRAME(MediaPlayer,"Media Player","Play","Media playback",0,false,true),
    FRAME(Photos,"Photos","Open","Photo library",0,false,false),
    FRAME(ImageViewer,"Image Viewer","Zoom","Image inspection",0,false,false),
    FRAME(Viewer,"Viewer","Open","Document viewer",0,false,false),
    FRAME(Browser,"Web Browser","Navigate","Network browser sandbox",0,true,true),
    FRAME(Notes,"Notes","New","Local notes",0,false,false),
    FRAME(Calendar,"Calendar","Add","Calendar events",0,false,false),
    FRAME(Clock,"Clock","Set","System clock",0,false,false),
    FRAME(NetworkManager,"Network Manager","Connect","Network interfaces and routes",0,true,true),
    FRAME(Bluetooth,"Bluetooth","Pair","Bluetooth controller",0,false,true),
    FRAME(Audio,"Audio","Apply","Audio devices and volume",0,false,true),
    FRAME(Display,"Display","Apply","Display modes and scaling",0,false,false),
    FRAME(UsersAccounts,"Users & Accounts","Add","Account management",0,false,true),
    FRAME(SecurityCenter,"Security Center","Scan","Security status",0,true,true),
    FRAME(Permissions,"Permissions","Apply","Capability grants",0,false,true),
    FRAME(Notifications,"Notifications","Clear","System notifications",0,false,true),
    FRAME(About,"About peaOS","Details","System information",0,false,false),
    FRAME(Help,"Help","Search","peaOS documentation",0,false,false),
    FRAME(PeaCloud,"peaCloud","Sync","Cloud synchronization",0,true,true),
    FRAME(MangaReader,"Manga Reader","Open","Manga library and network sources",0,true,true),
    FRAME(Development,"Development","Build","Native development workspace",0,false,true),
    FRAME(VisualStudioCode,"Visual Studio Code","Open","X90 Fusion development environment",0,false,true),
    FRAME(SandboxInterface,"Sandbox Interface","Launch","Isolated execution environment",0,false,true),
    FRAME(X90FusionManager,"X90 Fusion Manager","Launch","X90 runtime management",0,false,true),
    FRAME(TimeMachineManager,"TimeMachine Manager","Restore","Filesystem snapshot management",0,false,true),
    FRAME(MorphManager,"Morph Manager","Apply","System transformation manager",0,false,false),
    FRAME(CertificateYourApp,"Certificate Your App","Verify","Application certificate verification",0,true,true),
    FRAME(WebDistribution,"Web Distribution","Publish","Web distribution service",0,true,true)
};
#undef FRAME
}

#define WRAP(fn, id) AppFrame fn() { return kFrames[static_cast<uint32_t>(app_registry::AppId::id)]; }
WRAP(settings, Settings) WRAP(files, Files) WRAP(software_center, SoftwareCenter) WRAP(terminal, Terminal) WRAP(task_manager, TaskManager)
WRAP(system_monitor, SystemMonitor) WRAP(text_editor, TextEditor) WRAP(media_player, MediaPlayer) WRAP(photos, Photos) WRAP(image_viewer, ImageViewer)
WRAP(viewer, Viewer) WRAP(browser, Browser) WRAP(notes, Notes) WRAP(calendar, Calendar) WRAP(clock, Clock) WRAP(network_manager, NetworkManager)
WRAP(bluetooth, Bluetooth) WRAP(audio, Audio) WRAP(display, Display) WRAP(users_accounts, UsersAccounts) WRAP(security_center, SecurityCenter)
WRAP(permissions, Permissions) WRAP(notifications, Notifications) WRAP(about, About) WRAP(help, Help) WRAP(pea_cloud, PeaCloud) WRAP(manga_reader, MangaReader)
WRAP(development, Development) WRAP(visual_studio_code, VisualStudioCode) WRAP(sandbox_interface, SandboxInterface) WRAP(x90_fusion_manager, X90FusionManager)
WRAP(time_machine_manager, TimeMachineManager) WRAP(morph_manager, MorphManager) WRAP(certificate_your_app, CertificateYourApp) WRAP(web_distribution, WebDistribution)
#undef WRAP

const AppFrame* frame(app_registry::AppId id) {
    for (const AppFrame& f : kFrames) if (f.id == id) return &f;
    return nullptr;
}
}
