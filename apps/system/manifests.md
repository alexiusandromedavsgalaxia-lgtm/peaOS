# Bundled app manifests

Each application is intended to become a signed `.pea` package when the native userland/package loader is available.

| App | Runtime | Network | Isolation |
|---|---|---:|---:|
| Settings | native | optional | host service APIs |
| Photos | native | no | host filesystem permission |
| Viewer | native | no | host filesystem permission |
| Web Browser | X90 Fusion + Chromium | yes | web sandbox |
| Notes | native | no | host filesystem permission |
| peaCloud | native + cloud service | yes | cloud/network sandbox |
| Manga Reader | native | optional | content/network sandbox |
| Development | native + toolchain runtimes | optional | project sandbox |
| Visual Studio Code | X90 Fusion | optional | development sandbox |
| Sandbox Interface | native + guest services | optional | guest isolation |
