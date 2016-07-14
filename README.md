# LibEBC

Library for working with embedded bitcode.

## Build

```
$ git clone https://github.com/JDevlieghere/LibEBC.git && cd LibEBC
$ mkdir build && cd build
$ cmake ..
$ cmake --build .
```

## Usage

```
$ ebcutil fat.o
Mach-O 32-bit i386
       UUID: 16B4EDD1-4B58-35FB-849F-0CA0647D6C1C
     Dylibs: {SDKPATH}/usr/lib/libSystem.B.dylib
  Link opts: -execute -macosx_version_min 10.11.0 -e _main -executable_path build/i386.o
       File: Mach-O 32-bit i386_0.bc (Clang: -triple i386-apple-macosx10.11.0 -emit-obj -disable-llvm-optzns)
       File: Mach-O 32-bit i386_1.bc (Clang: -triple i386-apple-macosx10.11.0 -emit-obj -disable-llvm-optzns)

Mach-O 64-bit x86-64
       UUID: F6323CD5-E0DD-3E99-9D4A-B36B5A8E3E36
     Dylibs: {SDKPATH}/usr/lib/libSystem.B.dylib
  Link opts: -execute -macosx_version_min 10.11.0 -e _main -executable_path build/x86_64.o
       File: Mach-O 64-bit x86-64_0.bc (Clang: -triple x86_64-apple-macosx10.11.0 -emit-obj -disable-llvm-optzns)
       File: Mach-O 64-bit x86-64_1.bc (Clang: -triple x86_64-apple-macosx10.11.0 -emit-obj -disable-llvm-optzns)
```