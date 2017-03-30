# LibEBC

[![Build Status](https://travis-ci.org/JDevlieghere/LibEBC.svg?branch=master)](https://travis-ci.org/JDevlieghere/LibEBC)

Library and tool for retrieving embedded bitcode from binaries end libraries.
It supports all types of objects files (Mach-O, ELF, COFF, ...) as well as Mach-O
universal binaries, and static and dynamic libraries.

## EBC Util

`ebcutil` is a stand-alone command line tool for extracting embedded bitcode.

Information about the binary is printed as well as  metadata that is stored
together with the bitcode files. For universal Mach-O binaries this includes
linker flags and used dylibs. Objects not created by Apple LLVM contain only
the command line options passed to the clang frontend.

The bitcode files are created in the current working directory. They can be
processed by any tool accepting bitcode or you can use `llvm-dis` to convert
them into human-readable [LLVM assembly language](http://llvm.org/docs/LangRef.html).

Example of a **Mach-O universal binary** (fat binary):

```shell
$ file fat.o
fat.o: Mach-O universal binary with 2 architectures
fat.o (for architecture i386):    Mach-O executable i386
fat.o (for architecture x86_64):  Mach-O 64-bit executable x86_64

$ ebcutil fat.o
Mach-O 32-bit i386
  File name: fat.o
       Arch: x86
       UUID: 16B4EDD1-4B58-35FB-849F-0CA0647D6C1C
     Dylibs: {SDKPATH}/usr/lib/libSystem.B.dylib
  Link opts: -execute -macosx_version_min 10.11.0 -e _main -executable_path build/i386.o
    Bitcode: 2399E7B1-F4B6-4D55-916B-2EDAA799816E
    Bitcode: B3DEAEF5-4F4C-4FF1-95F0-7B5FDD2575F8
Mach-O 64-bit x86-64
  File name: fat.o
       Arch: x86_64
       UUID: F6323CD5-E0DD-3E99-9D4A-B36B5A8E3E36
     Dylibs: {SDKPATH}/usr/lib/libSystem.B.dylib
  Link opts: -execute -macosx_version_min 10.11.0 -e _main -executable_path build/x86_64.o
    Bitcode: DC97B259-9C3C-40D9-803C-8A841FB11DE3
    Bitcode: 7CC7D4AF-8750-4370-9D65-07CEBD720286
```


Example of an **ELF shared library**:

```shell
$ file lib.so
lib.so: ELF 32-bit LSB shared object, ARM, version 1 (SYSV), dynamically linked, not stripped

$ ebcutil lib.so
ELF32-arm-little
  File name: lib.so
       Arch: arm
       UUID: 00000000-0000-0000-0000-000000000000
    Bitcode: 565E3FBE-5B28-4428-B31B-3B3420CDA43A
        Clang: -triple thumbv7-none-linux-android -S -fembed-bitcode=all -disable-llvm-optzns ...
    Bitcode: C4C34416-D5A2-40BB-A85A-88380E4A1375
        Clang: -triple thumbv7-none-linux-android -S -fembed-bitcode=all -disable-llvm-optzns ...
```

Apple embeds more than just bitcode in the LLVM section. Example of a **Mach-O
with an embedded Mach-O and embedded Xar**:

```shell
$ file macho
macho: Mach-O 64-bit executable x86_64

$ ebcutil macho
Mach-O 64-bit x86-64
  File name: ASMExample
       Arch: x86_64
       UUID: 0A046594-A616-3CE3-9622-FF6D031BCA93
     Dylibs: {SDKPATH}/System/Library/Frameworks/Foundation.framework/Versions/C/Foundation ...
  Link opts: -execute -macosx_version_min 10.12.0 -e _main -executable_path ...
    Bitcode: 663A7A0C-1D29-4469-A68E-391BB16DD21A
      MachO: BC282643-364A-46A5-9690-F8226DB9F7DC
        Xar: C10CA2C2-EA04-4C71-AA17-9272DE85E0FD
```

You can also use `ebcutil` to check whether a given binary or library contains
bitcode. It prints the first object encountered without bitcode and exits with
a non-zero exit status.

```shell
$ ebcutil nobitcode.o
Error: No bitcode section in nobitcode.o
```

## Build

This project uses CMake as its build system. It has the following dependencies:

 - CMake 3.0 or later.
 - LibXML2
 - LibXar (optional, but required for Apple's embedded bitcode)
 - LLVM 3.9 or LLVM 4.0

```shell
$ git clone https://github.com/JDevlieghere/LibEBC.git && cd LibEBC
$ mkdir build && cd build
$ cmake ..
$ cmake --build .
```
