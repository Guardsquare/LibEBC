#pragma once

#include "ebc/EmbeddedFile.h"

#include <string>

namespace ebc {
class EmbeddedBitcode : public EmbeddedFile {
 public:
  EmbeddedBitcode(std::string file) : EmbeddedFile(std::move(file), EmbeddedFile::Type::Bitcode) {}
  ~EmbeddedBitcode() = default;
};
}
