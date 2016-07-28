#pragma once

#include "BitcodeFile.h"

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

  /// Returns the name associated with the Object embedding this bitcode archive.
  std::string GetName() const;
  void SetName(std::string name);

  /// Returns the commands associated with the embedded bitcode.
  std::string GetCmd() const;
  void SetCmd(std::string cmd);

  /// Returns the architecture of the Object embedding this bitcode archive.
  std::string GetArch() const;
  void SetArch(std::string arch);

  /// Returns the UUID associated with the Object embedding this bitcode archive.
  std::string GetUUID() const;
  void SetUuid(const std::uint8_t* uuid);

  /// Extract individual bitcode files from this container and return a vector of
  /// file names. This operation can be expensive as it decompresses each
  /// bitcode file.
  virtual std::vector<BitcodeFile> GetBitcodeFiles() const;

 protected:
  void SetData(const char* data, std::uint32_t size);
  std::pair<const char*, std::uint32_t> GetData() const;
  static constexpr int UUID_BYTE_LENGTH = 16;
  static constexpr int UUID_ASCII_LENGTH = 36;

 private:
  std::string _name;
  std::string _cmd;
  std::string _arch;
  std::array<std::uint8_t, 16> _uuid;

  char* _data;
  std::uint32_t _size;
};
}
