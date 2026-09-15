# peaOS application model

This directory defines the boundary for user applications.

Native applications will eventually be loaded as ELF executables into isolated user processes and communicate with the kernel through the syscall ABI and higher-level system services.

Compatibility runtimes will expose separate loaders and sandboxes:

- `compat/windows`: PE/Win32 runtime
- `compat/android`: APK/DEX/native Android runtime boundary
- `compat/macos`: portable Mach-O/application compatibility boundary and virtualization integration

No foreign executable is treated as a native peaOS binary merely because its filename looks familiar.
