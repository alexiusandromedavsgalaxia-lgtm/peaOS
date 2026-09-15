# peaOS application model

peaOS 1 Beta 1 has its own native application package formats.

## Native package formats

The supported native extensions are:

- `.pea` - standard peaOS application package.
- `.xpea` - X90-optimized native package for 64-bit peaOS applications.
- `.peac` - compact/native package intended for C++ applications and mixed C++/C# applications.

A native package is not just a renamed executable. The package contains a manifest plus the application's compiled native payload and required resources. The installer validates the package before it is registered with the system.

## Native languages

Native peaOS applications are compiled applications. The supported source languages for the native SDK are:

- **C++** compiled to the peaOS/X90 native ABI.
- **C# + C++** for mixed applications, where C++ provides native/system-facing components and C# runs through the peaOS managed-runtime boundary.

A package must declare its runtime and ABI in its manifest. Source files are development inputs and are not executed directly by the installer.

## Package layout

A `.pea`, `.xpea` or `.peac` package contains a structure equivalent to:

```text
MyApp.pea/
  manifest.pea
  bin/
    app.x90
  resources/
  runtime/
```

For a mixed C# + C++ application, the manifest identifies both the native X90 component and the managed component/runtime metadata.

## Installation pipeline

The future native package manager follows this pipeline:

1. identify `.pea`, `.xpea` or `.peac`;
2. parse and validate the manifest;
3. verify the target ABI and architecture;
4. verify package integrity/signature metadata;
5. install into the application store;
6. register the application with X90 Fusion;
7. create its sandbox and permissions;
8. expose it to the launcher and desktop.

Foreign formats such as Windows PE, Android APK and macOS Mach-O remain compatibility-runtime inputs. They are not silently converted into native peaOS packages.
