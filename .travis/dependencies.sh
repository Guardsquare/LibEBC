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

  wget http://llvm.org/releases/3.8.1/clang+llvm-3.8.1-x86_64-linux-gnu-ubuntu-14.04.tar.xz
  tar -xJf clang+llvm-3.8.1-x86_64-linux-gnu-ubuntu-14.04.tar.xz
  sudo mkdir -p $LLVM_HINT
  sudo mv -v clang+llvm-3.8.1-x86_64-linux-gnu-ubuntu-14.04/* $LLVM_HINT/
fi

