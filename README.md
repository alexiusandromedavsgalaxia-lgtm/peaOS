# peaOS 🫛

[![Certificates repository](https://img.shields.io/github/last-commit/alexiusandromedavsgalaxia-lgtm/peaOS.Officialy-certificates?label=certificates%20repo&logo=github)](https://github.com/alexiusandromedavsgalaxia-lgtm/peaOS.Officialy-certificates)
[![Certificates README](https://img.shields.io/badge/certificates-live%20registry-6A1B9A)](https://github.com/alexiusandromedavsgalaxia-lgtm/peaOS.Officialy-certificates/blob/main/README.md)

peaOS is being built as a complete desktop operating system, primarily in freestanding C++, with a tiny assembly entry layer where the CPU requires it.

## Live certificate registry

The certificate information is maintained in the separate **peaOS Officially Certificates** repository. This README does **not** copy or invent certificate records. The links below always point to the current `main` branch of the certificate repository:

- **Live certificate registry:** https://github.com/alexiusandromedavsgalaxia-lgtm/peaOS.Officialy-certificates
- **Live certificate status:** https://github.com/alexiusandromedavsgalaxia-lgtm/peaOS.Officialy-certificates/blob/main/README.md
- **Certificate source tree:** https://github.com/alexiusandromedavsgalaxia-lgtm/peaOS.Officialy-certificates/tree/main

The `last-commit` badge above is resolved from GitHub when the README is viewed, so it reflects the latest commit currently published in the certificate repository. The certificate repository remains the authoritative source; peaOS must not treat this README badge or a copied README section as a certificate database.

## CPU architecture: X90

**X90** is peaOS's name for its native **64-bit x86_64 platform target**. X90 is not a second CPU architecture. It is the platform/architecture name for the native peaOS system target.

## Current foundation

The project now has a structured kernel rather than a single-file demo:

- Multiboot/GRUB boot path
- X90 x86_64 long-mode entry
- 64-bit C++ kernel entry
- VGA console with COM1 serial mirroring
- bootstrap heap allocator
- native shell entry point
- multi-file kernel build
- CI build pipeline with boot-marker validation
- X90 CPUID and Multiboot memory probing
- minimum hardware policy: 2 GB RAM and 64 GB storage, with integrated graphics accepted
- privileged Initial Setup flow
- fail-closed activation service foundation
- native application package model
- X90 certificate policy foundation
- Development target model for peaOS, Windows, Android and macOS

## Initial Setup and activation

Activation is **not an installable application**. It is a privileged first-boot system flow.

The intended setup sequence is:

1. language;
2. region;
3. network;
4. hardware verification;
5. peaOS activation;
6. account and security;
7. desktop preparation.

The OS must not silently treat an unverified activation as active. The current bootstrap implementation is fail-closed until the real network protocol, cryptographic verification, persistent device identity and storage drivers exist.

## Native application packages

peaOS has its own native application formats:

- `.pea` for standard peaOS applications
- `.xpea` for X90-optimized native applications
- `.peac` for compact native applications, including mixed C++ + C# applications

Native applications are compiled for the peaOS/X90 ABI. Source code is not executed directly by the installer.

Every native package must carry a valid peaOS certificate/signature envelope before the OS will trust it. Certificate validation is performed by the OS trust layer rather than only by the application.

### Certificate Your App

**Certificate Your App** is the official free certificate-management application. The intended free certificate policy is:

- one certificate per company identity;
- up to five applications bound to the certificate;
- 90-day validity;
- manual renewal only;
- expired certificates stop affected applications from launching;
- renewal happens through the protected OS certificate service.

The current app implementation deliberately fails closed because real key storage and signature generation are not implemented yet.

### Web Distribution Program

A native package distributed publicly through the internet must also be enrolled in the **peaOS Web Distribution Program**. A local certificate cannot simply be promoted into web-distribution trust.

If a `.pea`, `.xpea` or `.peac` package is modified, unsigned, or distributed from an unauthorized web origin, the eventual installer/trust service rejects it.

## Development

The **Development** application is the unified development environment:

- peaOS → `.pea`, `.xpea`, `.peac`
- Windows → PE/Win32 `.exe`/`.dll`
- Android → APK/AAB
- macOS → source editing and remote compilation through a physically connected Mac

macOS development requires a Mac connected over USB-C **and Xcode already open**. peaOS does not redistribute or execute Xcode locally. Project files are synchronized over the cable, build instructions are sent to the Mac, Xcode builds with Apple's own toolchain, and the resulting artifact returns to Development.

## X90 features

- **X90 Morph**: context-aware desktop layouts with saved Morph states.
- **X90 TimeMachine FS**: temporal filesystem snapshots/version history.
- **X90 Fusion**: unified runtime boundary for native peaOS, Windows PE/Win32, Android APK and compatible macOS workloads, with shared integration services where technically possible.

## Architecture target

### Hardware and kernel

- BIOS + UEFI boot support
- **X90 x86_64 long mode**
- GDT/IDT, APIC, ACPI and SMP
- physical and virtual memory managers
- kernel heap
- processes, threads and scheduler
- syscalls and user mode
- PCI, USB, storage, audio, network and graphics drivers

### Desktop

- compositor/framebuffer graphics
- window manager
- desktop shell
- file manager
- terminal
- settings
- notifications
- application launcher
- package manager and installer

### System applications

The system application catalog is tracked in `apps/system-apps.manifest`. Initial Setup remains outside that catalog because it is part of the privileged boot flow.

### Windows compatibility

A dedicated PE/Win32 compatibility subsystem is planned, with Windows API translation, executable loading, filesystem integration, graphics/input bridges and sandboxing. Windows binaries are not treated as native X90 ELF programs.

### Android compatibility

APK discovery, package metadata, application sandboxing and an Android runtime/container layer are planned. The Android application ABI remains separate from the peaOS kernel process model.

### macOS compatibility

Portable application formats can be supported where their dependencies are implementable. Software requiring Apple's proprietary frameworks, signing infrastructure or Apple hardware cannot simply be copied into peaOS. Those workloads use an explicit virtualization/remote-build boundary instead of fake native compatibility.

## Development rule

A subsystem is not considered complete because its directory exists. It must compile, have a defined interface, and have a test or boot-time validation before being promoted.

The end goal is a real desktop OS that boots on physical PCs and presents native, Windows and Android applications through one integrated desktop, with macOS workloads handled through technically valid boundaries.
