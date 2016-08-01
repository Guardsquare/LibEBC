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

  # Build LLVM from source
  sudo mkdir -p /opt/llvm

  git clone http://llvm.org/git/llvm.git llvm
  cd llvm
  git checkout release_38
  cd ..

  mkdir build
  cd build
  cmake ../llvm -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/llvm
  make -j
  sudo make install
  cd ..
fi

