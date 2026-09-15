# peaOS Development

Unified development environment for four targets:

- peaOS: `.pea`, `.xpea`, `.peac`
- Windows: PE/Win32 applications
- Android: APK/AAB projects
- macOS: source editing and remote compilation through a physically connected Mac

## macOS rule

The macOS editor is unavailable until a Mac is connected over USB-C and Xcode is open on that Mac. peaOS does not execute or redistribute Xcode. Project files are synchronized over the cable, build commands are sent to Xcode, and the resulting application is returned to Development for the final sign/install step.

The current implementation provides the target model and connection/build state machine. USB transport, filesystem synchronization, Xcode protocol, signing, and the graphical editor still require their platform services.
