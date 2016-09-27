#!/bin/bash

if [[ $TRAVIS_OS_NAME == 'osx' ]]; then
  # Install dependencies for macOS
  brew update
  brew install libxml2

  # Get LLVM 3.9.0
  wget http://llvm.org/releases/3.9.0/clang+llvm-3.9.0-x86_64-apple-darwin.tar.xz
  tar -xJf clang+llvm-3.9.0-x86_64-apple-darwin.tar.xz
  sudo mkdir -p "$LLVM_HINT"
  sudo mv -v clang+llvm-3.9.0-x86_64-apple-darwin/* "$LLVM_HINT/"
else
  # Install dependencies for Linux
  sudo apt-get -qq update
  sudo apt-get install -y libxml2-dev

  # Use CMake 3.6.1
  wget https://cmake.org/files/v3.6/cmake-3.6.1-Linux-x86_64.tar.gz
  tar xf cmake-3.6.1-Linux-x86_64.tar.gz
  export PATH=$(pwd)/cmake-3.6.1-Linux-x86_64/bin:$PATH

  # Get LLVM 3.9.0
  wget http://llvm.org/releases/3.9.0/clang+llvm-3.9.0-x86_64-linux-gnu-ubuntu-14.04.tar.xz
  tar -xJf clang+llvm-3.9.0-x86_64-linux-gnu-ubuntu-14.04.tar.xz
  sudo mkdir -p "$LLVM_HINT"
  sudo mv -v clang+llvm-3.9.0-x86_64-linux-gnu-ubuntu-14.04/* "$LLVM_HINT/"
fi

