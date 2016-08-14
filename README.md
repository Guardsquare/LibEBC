# LibEBC

[![Build Status](https://travis-ci.org/JDevlieghere/LibEBC.svg?branch=master)](https://travis-ci.org/JDevlieghere/LibEBC)

Library and tool for retrieving embedded bitcode from binaries end libraries.
It supports all types of objects files (MachO, ELF, COFF, ...) as well as MachO
universal binaries, and static and dynamic libraries.

## EBC Util

`ebcutil` is a stand-alone command line tool for extracting embedded bitcode.

Information about the binary is printed as well as  metadata that is stored
together with the bitcode files. For universal MachO binaries this includes
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

Example of an **ELF shared library**:

```shell
$ file lib.so
lib.so: ELF 32-bit LSB shared object, ARM, version 1 (SYSV), dynamically linked, not stripped

$ ebcutil lib.so
ELF32-arm-little
  File name: lib.so
       Arch: arm
       UUID: 00000000-0000-0000-0000-000000000000
    Bitcode: 0.bc
        Clang: -triple thumbv7-none-linux-android -S -fembed-bitcode=all -disable-llvm-optzns ...
    Bitcode: 1.bc
        Clang: -triple thumbv7-none-linux-android -S -fembed-bitcode=all -disable-llvm-optzns ...
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

 - CMake 3.0
 - LibXML2
 - LibXar (optional, but required for Mach-O)
 - LLVM 3.8

```shell
$ git clone https://github.com/JDevlieghere/LibEBC.git && cd LibEBC
$ mkdir build && cd build
$ cmake ..
$ cmake --build .
```
