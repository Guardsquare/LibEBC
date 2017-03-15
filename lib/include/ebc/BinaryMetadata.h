#pragma once

#include <array>
#include <string>

namespace ebc {

/// Abstraction of the meta data linked to a binary object or archive.
class BinaryMetadata {
 public:
  BinaryMetadata();
  std::string GetFileFormatName() const;
  void SetFileFormatName(std::string fileFormatName);

  std::string GetFileName() const;
  void SetFileName(std::string fileName);

  std::string GetArch() const;
  void SetArch(std::string arch);

  std::string GetUUID() const;
  void SetUuid(const std::uint8_t* uuid);

 private:
  std::string _fileFormatName;
  std::string _fileName;
  std::string _arch;
  std::array<std::uint8_t, 16> _uuid;
};
}
