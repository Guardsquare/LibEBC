# LibEBC

[![Build Status](https://travis-ci.org/JDevlieghere/LibEBC.svg?branch=master)](https://travis-ci.org/JDevlieghere/LibEBC)

Library for obtaining bitcode embedded in object files. In theory it should
work with all types of objects files (MachO, ELF, COFF, ...) as well as with
MachO universal binaries.

Embedded bitcode was originally added by Apple for MachO binaries and later
[upstreamed to clang](http://lists.llvm.org/pipermail/llvm-dev/2016-February/094851.html).
More information can be found in [this blogpost](https://jonasdevlieghere.com/embedded-bitcode/)
I wrote.

## Usage

```
$ ebcutil fat.o
Mach-O 32-bit i386
  File name: fat.o
       Arch: x86
       UUID: 16B4EDD1-4B58-35FB-849F-0CA0647D6C1C
     Dylibs: {SDKPATH}/usr/lib/libSystem.B.dylib
  Link opts: -execute -macosx_version_min 10.11.0 -e _main -executable_path build/i386.o
    Bitcode: 0.bc
    Bitcode: 1.bc
Mach-O 64-bit x86-64
  File name: fat.o
       Arch: x86_64
       UUID: F6323CD5-E0DD-3E99-9D4A-B36B5A8E3E36
     Dylibs: {SDKPATH}/usr/lib/libSystem.B.dylib
  Link opts: -execute -macosx_version_min 10.11.0 -e _main -executable_path build/x86_64.o
    Bitcode: 2.bc
    Bitcode: 3.bc
```

## Build

```
$ git clone https://github.com/JDevlieghere/LibEBC.git && cd LibEBC
$ mkdir build && cd build
$ cmake ..
$ cmake --build .
```
