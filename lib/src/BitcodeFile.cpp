#include "ebc/BitcodeFile.h"

#include <cstdio>

namespace ebc {

BitcodeFile::BitcodeFile(std::string name) : _name(name) {}

std::string BitcodeFile::GetName() const {
  return _name;
}

const std::vector<std::string>& BitcodeFile::GetCommands() const {
  return _commands;
}

void BitcodeFile::SetCommands(const std::vector<std::string>& commands) {
  _commands = commands;
}

void BitcodeFile::Remove() {
  std::remove(_name.c_str());
}
}
