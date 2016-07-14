#pragma once

#include "BitcodeFile.h"
#include "BitcodeMetadata.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ebc {
class BitcodeArchive {
 public:
  BitcodeArchive(std::string name, const std::uint8_t uuid[16], const char* data, std::uint32_t size);
  BitcodeArchive(BitcodeArchive&& bitcodeArchive);
  ~BitcodeArchive();

  std::string GetName() const;
  std::string GetUUID() const;

  /// Write data to file. If no file name is provided, the name of this
  /// BitcodeArchive will be used, followed by the xar extension.
  std::string WriteXarToFile(std::string fileName = "") const;

  const BitcodeMetadata& GetMetadata() const;

  std::vector<BitcodeFile> GetBitcodeFiles() const;

 private:
  std::string GetMetadataXml() const;

  std::string _name;
  std::uint8_t _uuid[16];
  char* _data;
  std::uint32_t _size;
  std::unique_ptr<BitcodeMetadata> _metadata;
};
}
