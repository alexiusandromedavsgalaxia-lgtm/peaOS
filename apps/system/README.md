# peaOS bundled applications

Beta 1 exposes the complete bundled application suite through the desktop registry. The registry is an application contract; executable third-party engines are never faked as native code.

## Included

- Settings: hardware, display, network, accounts, permissions, activation, updates and privacy controls.
- Photos: image library, metadata, folders, basic transforms and slideshow.
- Viewer: generic document/media viewer with MIME/type dispatch.
- Web Browser: tabs, history, downloads, permissions and isolated web content. Chromium is the preferred engine integration. A Chromium binary is not claimed to be present until the X90 runtime actually loads it.
- Notes: local notes, autosave, search, folders and TimeMachine snapshots.
- peaCloud: cloud files, sync state, conflict handling and offline cache. Authentication and transport belong to the network/security services.
- Manga Reader: local/remote library, chapter navigation, page ordering and reading position.
- Development: editor, project explorer, terminals, compilers, debugger, UI designer, SDK/dependency manager, emulators/sandboxes, packaging and publishing.
- Visual Studio Code: permanently installed as a desktop integration entry. A compatible VS Code runtime is launched through X90 Fusion rather than pretending a foreign executable is native.
- Sandbox Interface: isolated peaOS guest with separate process, filesystem, permissions and network policy. It is a guest sandbox, not a second host kernel.

## Runtime rule

The base image may carry manifests, icons and launch metadata. Executable engines and foreign applications are loaded only when their compatibility runtime is available and package authorization succeeds.

Native `.pea/.xpea/.peac` applications remain subject to the peaOS certificate and permission services.
