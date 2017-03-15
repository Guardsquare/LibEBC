#pragma once

#include "ebc/EmbeddedFile.h"

#include <memory>
#include <string>

namespace ebc {
class BitcodeContainer;
class EmbeddedXar : public EmbeddedFile {
 public:
  EmbeddedXar(std::string file) : EmbeddedFile(std::move(file), EmbeddedFile::Type::Xar) {}
  ~EmbeddedXar() = default;

  std::unique_ptr<BitcodeContainer> GetAsBitcodeArchive() const;
};
}
