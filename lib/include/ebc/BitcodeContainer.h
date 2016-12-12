#pragma once

#include "ebc/BinaryMetadata.h"

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ebc {
class BitcodeFile;
class BitcodeContainer {
 public:
  BitcodeContainer(const char* data, std::uint32_t size);

  BitcodeContainer(BitcodeContainer&& bitcodeArchive) noexcept;

  virtual ~BitcodeContainer();

  /// Indicates whether the given bitcode container is a bitcode archive.
  ///
  /// @return False.
  virtual bool IsArchive() const;

  /// Indicates whether the given bitcode container is merely a placeholder.
  ///
  /// @return  True if data is empty.
  bool IsEmpty() const;

  /// Get the commands passed to the compiler front end.
  ///
  /// @return The commands passed to the compiler front end.
  const std::vector<std::string>& GetCommands() const;

  /// Set the commands passed to the compiler front end.
  ///
  /// @param cmd The commands passed to the compiler front end.
  void SetCommands(const std::vector<std::string>& cmd);

  /// Get the binary meta data associated with this bitcode container.
  ///
  /// @return The binary meta data.
  BinaryMetadata& GetBinaryMetadata();

  /// Set the binary meta data associated with this bitcode container.
  ///
  /// @return  The binary meta data.
  const BinaryMetadata& GetBinaryMetadata() const;

  /// Extract individual bitcode files from this container and return a vector
  /// of file names. This operation can be expensive as it decompresses each
  /// bitcode file.
  ///
  /// @param extract Optional paramter indicating whether different bitcode
  ///                files should be extracted and written to file.
  ///
  /// @return A vector of bitcode files.
  virtual std::vector<BitcodeFile> GetBitcodeFiles(bool extract = false) const;

 protected:
  void SetData(const char* data, std::uint32_t size) noexcept;
  std::pair<const char*, std::uint32_t> GetData() const;

 private:
  std::vector<std::uint32_t> GetBitcodeFileOffsets() const;

  char* _data;
  std::uint32_t _size;

  std::vector<std::string> _commands;
  BinaryMetadata _binaryMetadata;
};
}
