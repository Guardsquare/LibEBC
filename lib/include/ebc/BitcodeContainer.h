#pragma once

#include "ebc/BinaryMetadata.h"
#include "ebc/BitcodeFile.h"

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ebc {
class BitcodeMetadata;
class BitcodeContainer {
 public:
  BitcodeContainer(const char* data, std::uint32_t size);

  BitcodeContainer(BitcodeContainer&& bitcodeArchive);

  virtual ~BitcodeContainer();

  virtual bool IsArchive() const;

  /// Returns the commands associated with the embedded bitcode.
  const std::vector<std::string>& GetCommands() const;
  void SetCommands(const std::vector<std::string>& cmd);

  BinaryMetadata& GetBinaryMetadata();
  const BinaryMetadata& GetBinaryMetadata() const;

  /// Extract individual bitcode files from this container and return a vector of
  /// file names. This operation can be expensive as it decompresses each
  /// bitcode file.
  virtual std::vector<BitcodeFile> GetBitcodeFiles(std::string prefix = "") const;

 protected:
  void SetData(const char* data, std::uint32_t size);
  std::pair<const char*, std::uint32_t> GetData() const;

 private:
  std::vector<std::uint32_t> GetBitcodeFileOffsets() const;

  char* _data;
  std::uint32_t _size;

  std::vector<std::string> _commands;
  BinaryMetadata _binaryMetadata;
};
}
