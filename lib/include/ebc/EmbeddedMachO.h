#pragma once

#include "ebc/EmbeddedFile.h"

#include <string>

namespace ebc {
class EmbeddedMachO : public EmbeddedFile {
 public:
  EmbeddedMachO(std::string file) : EmbeddedFile(std::move(file), EmbeddedFile::Type::MachO) {}
  ~EmbeddedMachO() = default;
};
}
