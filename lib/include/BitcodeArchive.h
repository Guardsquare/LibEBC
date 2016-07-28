#pragma once

#include "BitcodeContainer.h"
#include "BitcodeFile.h"
#include "BitcodeMetadata.h"

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ebc {
class BitcodeArchive : public BitcodeContainer {
 public:
  BitcodeArchive(const char* data, std::uint32_t size);

  BitcodeArchive(BitcodeArchive&& bitcodeArchive);

  virtual bool IsArchive() const override;

  /// Write data to file. If no file name is provided, the name of this
  /// BitcodeArchive will be used, followed by the xar extension.
  std::string WriteXarToFile(std::string fileName = "") const;

  /// Return the MetaData contained in this bitcode archive. This operation is
  /// cheap as the heavy lifting occurs at construction time.
  const BitcodeMetadata& GetMetadata() const;

  /// Extract individual bitcode files from this archive and return a vector of
  /// file names. This operation can be expensive as it decompresses each
  /// bitcode file.
  std::vector<BitcodeFile> GetBitcodeFiles() const override;

 private:
  void SetMetadata();
  /// Serializes XAR metadata to XML. Beware that this operation is expensive as
  /// both the archive and the metadata XML are intermediately written to disk.
  std::string GetMetadataXml() const;

  std::unique_ptr<BitcodeMetadata> _metadata;
};
}
