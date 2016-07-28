#include "ebc/BitcodeFile.h"

#include <cstdio>

namespace ebc {

BitcodeFile::BitcodeFile(std::string name) : _name(name) {}

std::string BitcodeFile::GetName() const {
  return _name;
}

const std::vector<std::string>& BitcodeFile::GetClangCommands() const {
  return _clangCommands;
}

void BitcodeFile::SetClangCommands(std::vector<std::string>& clangCommands) {
  _clangCommands = clangCommands;
}

const std::vector<std::string>& BitcodeFile::GetSwiftCommands() const {
  return _swiftCommands;
}

void BitcodeFile::SetSwiftCommands(std::vector<std::string>& swiftCommands) {
  _swiftCommands = swiftCommands;
}

void BitcodeFile::Remove() {
  std::remove(_name.c_str());
}
}
