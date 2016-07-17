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
  /// Create a bitcode archive from the name and UUID associated with the
  /// embedding MachO object, together with a buffer containing the XAR archive.
  BitcodeArchive(std::string name, const std::uint8_t* uuid, const char* data, std::uint32_t size);

  BitcodeArchive(BitcodeArchive&& bitcodeArchive);

  ~BitcodeArchive();

  /// Returns the name associated with the MachO embedding this bitcode archive.
  std::string GetName() const;

  /// Returns the UUID associated with the MachO embedding this bitcode archive.
  std::string GetUUID() const;

  /// Write data to file. If no file name is provided, the name of this
  /// BitcodeArchive will be used, followed by the xar extension.
  std::string WriteXarToFile(std::string fileName = "") const;

  /// Return the MetaData contained in this bitcode archive. This operation is
  /// cheap as the heavy lifting occurs at construction time.
  const BitcodeMetadata& GetMetadata() const;

  /// Extract individual bitcode files from this archive and return a vector of
  /// file names. This operation can be expensive as it decompresses each
  /// bitcode file.
  std::vector<BitcodeFile> GetBitcodeFiles() const;

 private:
  /// Serializes XAR metadata to XML. Beware that this operation is expensive as
  /// both the archive and the metadata XML are intermediately written to disk.
  std::string GetMetadataXml() const;

  static constexpr int UUID_BYTE_LENGTH = 16;
  static constexpr int UUID_ASCII_LENGTH = 36;

  std::string _name;
  std::uint8_t _uuid[UUID_BYTE_LENGTH];
  char* _data;
  std::uint32_t _size;
  std::unique_ptr<BitcodeMetadata> _metadata;
};
}
