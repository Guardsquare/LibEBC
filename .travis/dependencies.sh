#!/bin/bash

if [[ $TRAVIS_OS_NAME == 'osx' ]]; then
  # Install dependencies for macOS
  brew update
  brew install libxml2
  brew install llvm
else
  # Install dependencies for Linux
  sudo apt-get -qq update
  sudo apt-get install -y libxml2-dev

  # Use CMake 3.6.1
  wget https://cmake.org/files/v3.6/cmake-3.6.1-Linux-x86_64.tar.gz
  tar xf cmake-3.6.1-Linux-x86_64.tar.gz
  export PATH=$(pwd)/cmake-3.6.1-Linux-x86_64/bin:$PATH

  # Get LLVM 3.8.1
  wget http://llvm.org/releases/3.8.1/clang+llvm-3.8.1-x86_64-linux-gnu-ubuntu-14.04.tar.xz
  tar -xJf clang+llvm-3.8.1-x86_64-linux-gnu-ubuntu-14.04.tar.xz
  sudo mkdir -p $LLVM_HINT
  sudo mv -v clang+llvm-3.8.1-x86_64-linux-gnu-ubuntu-14.04/* $LLVM_HINT/
fi

