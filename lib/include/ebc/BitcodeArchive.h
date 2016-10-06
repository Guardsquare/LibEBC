#pragma once

#include "ebc/BitcodeContainer.h"

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ebc {

class BitcodeFile;
class BitcodeMetadata;

/// A bitcode archive is a special kind of bitcode container created by Apple's
/// version of LLVM. It contains a XAR archive with the (unlinked) bitcode
/// files. Additional metadata is part of the archive's table of content.
class BitcodeArchive : public BitcodeContainer {
 public:
  BitcodeArchive(const char* data, std::uint32_t size);

  BitcodeArchive(BitcodeArchive&& bitcodeArchive) noexcept;

  /// Indicates whether the given bitcode container is a bitcode archive.
  ///
  /// @return True.
  virtual bool IsArchive() const override;

  /// Write container data to file. If no file name is provided, the file
  /// format name of the binary will be used, followed by the xar extension.
  /// This works even when compiled without xar support.
  ///
  /// @param fileName Optional file name for the XAR archive.
  ///
  /// @return The path to the XAR archive file.
  std::string WriteXarToFile(std::string fileName = "") const;

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
  /// @param extract Optional paramter indicating whether different bitcode
  ///                files should be extracted and written to file.
  ///
  /// @return A vector of bitcode files.
  std::vector<BitcodeFile> GetBitcodeFiles(bool extract = false) const override;

 private:
  void SetMetadata() noexcept;

  /// Serializes XAR metadata to XML. Beware that this operation is expensive as
  /// both the archive and the metadata XML are intermediately written to disk.
  std::string GetMetadataXml() const noexcept;

  std::unique_ptr<BitcodeMetadata> _metadata;
};
}
