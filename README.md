# peaOS 🫛

peaOS is a small x86 operating-system project with a freestanding C++ kernel and a minimal Multiboot entry point.

## Current boot path

BIOS/GRUB → Multiboot header → 32-bit entry stub → C++ kernel → VGA text output → halt loop.

The repository is intentionally starting from a genuinely bootable foundation rather than a fake `mainActivity` or a userspace program pretending to be an OS.

## Build

Requirements on Debian/Ubuntu:

```bash
sudo apt install g++ gcc-multilib binutils nasm grub-pc-bin grub-common xorriso qemu-system-x86
make
make run
```

The resulting `build/peaOS.iso` is a bootable GRUB ISO that can be tested in QEMU or written to a spare USB for real-hardware testing.

## Roadmap

- Interrupt descriptor table and PIC
- Physical memory manager
- Paging and virtual memory
- Keyboard driver
- Timer and scheduler
- Heap allocator
- Filesystem layer
- PCI enumeration
- Framebuffer graphics
- Window manager and userspace
- Native peaOS command shell
- 64-bit long-mode kernel

The kernel is C++ where the hardware-facing environment permits it. A tiny assembly entry point is retained because the processor needs a machine-code entry path before C++ can execute.
