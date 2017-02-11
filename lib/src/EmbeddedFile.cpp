#include "ebc/EmbeddedFile.h"

#include <cstdio>
#include <utility>

namespace ebc {

EmbeddedFile::EmbeddedFile(std::string name) : _name(std::move(name)), _type(EmbeddedFile::Type::File) {}
EmbeddedFile::EmbeddedFile(std::string name, EmbeddedFile::Type type) : _name(std::move(name)), _type(type) {}

std::string EmbeddedFile::GetName() const {
  return _name;
}

const std::vector<std::string>& EmbeddedFile::GetCommands() const {
  return _commands;
}

void EmbeddedFile::SetCommands(const std::vector<std::string>& commands) {
  _commands = commands;
}

EmbeddedFile::Type EmbeddedFile::GetType() const {
  return _type;
}

void EmbeddedFile::Remove() const {
  std::remove(_name.c_str());
}
}  // namespace ebc
