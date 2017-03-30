#!/bin/bash

if [[ $TRAVIS_OS_NAME == 'osx' ]]; then
  # Install dependencies for macOS
  brew update
  brew install libxml2
  brew link -f libxml2

  # Get LLVM
  wget "http://llvm.org/releases/$LLVM_VERSION/clang+llvm-$LLVM_VERSION-x86_64-apple-darwin.tar.xz"
  tar -xJvf "clang+llvm-$LLVM_VERSION-x86_64-apple-darwin.tar.xz"
  sudo mkdir -p "$LLVM_HINT"
  sudo mv -v "clang+llvm-$LLVM_VERSION-x86_64-apple-darwin/"* "$LLVM_HINT/"
else
  # Install dependencies for Linux
  sudo apt-get -qq update
  sudo apt-get install -y libxml2-dev uuid-dev

  # Use CMake 3.6.1
  wget https://cmake.org/files/v3.6/cmake-3.6.1-Linux-x86_64.tar.gz
  tar xf cmake-3.6.1-Linux-x86_64.tar.gz
  export PATH=$(pwd)/cmake-3.6.1-Linux-x86_64/bin:$PATH

  # Get LLVM
  wget "http://llvm.org/releases/$LLVM_VERSION/clang+llvm-$LLVM_VERSION-x86_64-linux-gnu-ubuntu-14.04.tar.xz"
  tar -xJvf "clang+llvm-$LLVM_VERSION-x86_64-linux-gnu-ubuntu-14.04.tar.xz"
  sudo mkdir -p "$LLVM_HINT"
  sudo mv -v "clang+llvm-$LLVM_VERSION-x86_64-linux-gnu-ubuntu-14.04/"* "$LLVM_HINT/"
fi

