# peaOS bundled applications

The Beta 1 application suite is defined here. These are native peaOS applications and services, not kernel placeholders.

## Included

- Settings: hardware, display, network, accounts, permissions, activation, updates and privacy controls.
- Photos: image library, metadata, folders, basic transforms and slideshow.
- Viewer: generic document/media viewer with MIME/type dispatch.
- Web Browser: browser frontend with tabs, history, downloads, permissions and sandboxed web content. Chromium is the preferred engine integration; the OS does not ship an imaginary native Chromium implementation in the kernel.
- Notes: local notes with autosave, search, folders and TimeMachine snapshots.
- peaCloud: cloud files, sync state, conflict handling and offline cache. Authentication and transport are delegated to the network/security services.
- Manga Reader: local and remote chapter/library reader with page ordering, reading position and archive support.
- Visual Studio Code: permanent Development integration entry. The peaOS app launches the supported VS Code/remote-code runtime rather than pretending a foreign binary is native.
- Sandbox Interface: launches an isolated peaOS guest environment with its own process, filesystem, permissions and network policy. It is a sandboxed guest, not a second copy of the host kernel.

## Runtime rule

Application metadata may be present in the base image, but executable third-party runtimes are loaded only when their compatibility layer is available and their package authorization succeeds.

This directory documents the application contracts while the native userland and package loader are brought online.
