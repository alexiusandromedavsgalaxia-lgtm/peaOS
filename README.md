# peaOS 🫛

peaOS is being built as a complete desktop operating system, primarily in freestanding C++, with a tiny assembly entry layer where the CPU requires it.

## CPU architecture: X90

**X90** is peaOS's name for its native **64-bit x86_64 platform target**. From this point forward, the 64-bit kernel, boot image, build target and platform-specific code are identified as X90.

X90 is not a second CPU architecture. It is peaOS's platform/architecture name for its x86_64 system target.

## Current foundation

The project now has a structured kernel instead of a single-file demo:

- Multiboot/GRUB boot path
- X90 x86_64 long-mode entry
- 64-bit C++ kernel entry
- VGA console with scrolling
- bootstrap heap allocator
- native shell entry point
- multi-file kernel build
- CI build pipeline

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

### Applications

Native peaOS applications will use a stable C++ system API and application SDK.

### Windows compatibility

A dedicated PE/Win32 compatibility subsystem is planned, with Windows API translation, executable loading, filesystem integration, graphics/input bridges and sandboxing. A Wine-style userspace can be integrated rather than pretending that Windows binaries are native ELF programs.

### Android compatibility

APK discovery, package metadata, application sandboxing and an Android runtime/container layer are planned. The design separates the Android application ABI from the peaOS kernel so Android applications can run without polluting the native process model.

### macOS compatibility

Portable application formats can be supported where their dependencies are implementable. Software requiring Apple's proprietary frameworks, signing infrastructure or Apple hardware cannot simply be copied into peaOS. For those workloads, the architecture reserves a virtualization boundary instead of claiming impossible native compatibility.

## Development rule

A subsystem is not considered complete because its directory exists. It must compile, have a defined interface, and have a test or boot-time validation before being promoted.

The end goal is a real desktop OS that boots on physical PCs and presents native, Windows and Android applications through one integrated desktop, with macOS compatibility handled through technically and legally valid mechanisms.
