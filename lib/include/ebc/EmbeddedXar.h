#pragma once

#include "ebc/EmbeddedFile.h"

#include <memory>
#include <string>

namespace ebc {
class BitcodeArchive;
class EmbeddedXar : public EmbeddedFile {
 public:
  EmbeddedXar(std::string file) : EmbeddedFile(std::move(file), EmbeddedFile::Type::Xar) {}
  ~EmbeddedXar() = default;

  std::unique_ptr<BitcodeArchive> GetAsBitcodeArchive() const;
};
}
