#pragma once

#include "ebc/BitcodeContainer.h"

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ebc {

class EmbeddedFile;
class BitcodeMetadata;

/// A bitcode archive is a special kind of bitcode container created by Apple's
/// version of LLVM. It contains a XAR archive with the (unlinked) bitcode
/// files. Additional metadata is part of the archive's table of content.
class BitcodeArchive : public BitcodeContainer {
 public:
  BitcodeArchive(const char* data, std::size_t size);

  BitcodeArchive(BitcodeArchive&& bitcodeArchive) noexcept;

  virtual ~BitcodeArchive() = default;

  /// Indicates whether the given bitcode container is a bitcode archive.
  ///
  /// @return True.
  virtual bool IsArchive() const override;

  /// Write container data to file.
  ///
  /// @return The path to the XAR archive file.
  std::string WriteXarToFile() const;

  /// Return the MetaData contained in this bitcode archive. This operation is
  /// cheap as the heavy lifting occurs at construction time. Metadata is empty
  /// if not compiled with xar support.
  ///
  /// @return The meta data.
  const BitcodeMetadata& GetMetadata() const;

  /// Extract individual bitcode files from this archive and return a vector of
  /// file names. This operation can be expensive as it decompresses each
  /// bitcode file. The result is empty if not compiled with xar support.
  ///
  /// @return A vector of bitcode files.
  std::vector<std::unique_ptr<EmbeddedFile>> GetEmbeddedFiles() const override;

  static std::unique_ptr<BitcodeContainer> BitcodeArchiveFromFile(std::string path);

 private:
  void SetMetadata() noexcept;

  /// Serializes XAR metadata to XML. Beware that this operation is expensive as
  /// both the archive and the metadata XML are intermediately written to disk.
  std::string GetMetadataXml() const noexcept;

  std::unique_ptr<BitcodeMetadata> _metadata;
};
}
