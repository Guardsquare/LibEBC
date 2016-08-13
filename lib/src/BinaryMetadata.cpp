#include "ebc/BinaryMetadata.h"

#include "ebc/util/UUID.h"

#include <algorithm>

namespace ebc {

std::string BinaryMetadata::GetFileFormatName() const {
  return _fileFormatName;
}

void BinaryMetadata::SetFileFormatName(std::string fileFormatName) {
  _fileFormatName = fileFormatName;
}

std::string BinaryMetadata::GetFileName() const {
  return _fileName;
}

void BinaryMetadata::SetFileName(std::string fileName) {
  _fileName = fileName;
}

std::string BinaryMetadata::GetArch() const {
  return _arch;
}

void BinaryMetadata::SetArch(std::string arch) {
  _arch = arch;
}

void BinaryMetadata::SetUuid(const std::uint8_t *uuid) {
  if (uuid != nullptr) {
    std::copy_n(uuid, _uuid.size(), _uuid.begin());
  }
}

std::string BinaryMetadata::GetUUID() const {
  return util::uuid::UuidToString(_uuid);
}
}  // namespace ebc
