#pragma once

#include "ebc/BitcodeType.h"
#include "ebc/EmbeddedFile.h"

#include <string>

namespace ebc {
class EmbeddedBitcode : public EmbeddedFile {
 public:
  EmbeddedBitcode(std::string file, BitcodeType bitcodeType)
      : EmbeddedFile(std::move(file), EmbeddedFile::Type::Bitcode), _bitcodeType(bitcodeType) {}
  EmbeddedBitcode(std::string file)
      : EmbeddedFile(std::move(file), EmbeddedFile::Type::Bitcode), _bitcodeType(BitcodeType::Bitcode) {}
  ~EmbeddedBitcode() = default;

  BitcodeType GetBitcodeType() {
    return _bitcodeType;
  }

 private:
  BitcodeType _bitcodeType;
};
}
