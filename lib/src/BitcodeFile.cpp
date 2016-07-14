#include "BitcodeFile.h"

#include <cstdio>

namespace ebc {

BitcodeFile::BitcodeFile(std::string name) : _name(name) {}

void BitcodeFile::SetClangCommands(std::vector<std::string>& clangCommands) {
  _clangCommands = clangCommands;
}

void BitcodeFile::Remove() {
  std::remove(_name.c_str());
}

std::string BitcodeFile::GetName() const {
  return _name;
}

const std::vector<std::string>& BitcodeFile::GetClangCommands() const {
  return _clangCommands;
}
}
