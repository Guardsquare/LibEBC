#include "ebc/BitcodeContainer.h"

#include "ebc/EmbeddedBitcode.h"
#include "ebc/util/Bitcode.h"
#include "ebc/util/UUID.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <streambuf>

namespace ebc {

BitcodeContainer::BitcodeContainer(const char *data, std::uint32_t size)
    : _data(nullptr), _size(size), _commands(), _prefix() {
  SetData(data, size);
}

BitcodeContainer::BitcodeContainer(BitcodeContainer &&bitcodeContainer) noexcept
    : _data(nullptr)
    , _size(bitcodeContainer._size)
    , _commands(bitcodeContainer._commands)
    , _binaryMetadata(bitcodeContainer._binaryMetadata)
    , _prefix() {
  SetData(bitcodeContainer._data, bitcodeContainer._size);
  bitcodeContainer._data = nullptr;
}

BitcodeContainer::~BitcodeContainer() {
  if (_data != nullptr) {
    delete[] _data;
    _data = nullptr;
  }
}

bool BitcodeContainer::IsArchive() const {
  return false;
}

bool BitcodeContainer::IsEmpty() const {
  return _size == 0;
}

const std::vector<std::string> &BitcodeContainer::GetCommands() const {
  return _commands;
}

void BitcodeContainer::SetCommands(const std::vector<std::string> &commands) {
  _commands = commands;
}

void BitcodeContainer::SetData(const char *data, std::uint32_t size) noexcept {
  if (size > 0) {
    _data = new char[size];
    std::copy(data, data + size, _data);
  }
}

std::pair<const char *, std::uint32_t> BitcodeContainer::GetData() const {
  return std::make_pair(_data, _size);
}

BinaryMetadata &BitcodeContainer::GetBinaryMetadata() {
  return _binaryMetadata;
}

const BinaryMetadata &BitcodeContainer::GetBinaryMetadata() const {
  return _binaryMetadata;
}

std::vector<std::unique_ptr<EmbeddedFile>> BitcodeContainer::GetEmbeddedFiles() const {
  // Magic number is 4 bytes long. If less than four bytes are available there
  // is no bitcode. Likely only a bitcode marker was embedded.
  if (IsEmpty() || _size < 4) {
    return {};
  }

  std::vector<std::unique_ptr<EmbeddedFile>> files;
  auto offsets = GetEmbeddedFileOffsets();
  for (std::uint32_t i = 0; i < offsets.size() - 1; ++i) {
    auto begin = offsets[i];
    auto end = offsets[i + 1];
    auto size = end - begin;

    auto fileName = _prefix + util::uuid::UuidToString(util::uuid::GenerateUUID());
    util::bitcode::WriteToFile(_data + begin, size, fileName);

    auto bitcodeFile = std::unique_ptr<EmbeddedFile>(new EmbeddedBitcode(fileName));
    bitcodeFile->SetCommands(_commands);
    files.push_back(std::move(bitcodeFile));
  }

  return files;
}

std::vector<std::uint32_t> BitcodeContainer::GetEmbeddedFileOffsets() const {
  std::vector<std::uint32_t> offsets;
  for (std::uint32_t i = 0; i < _size - 3; ++i) {
    if (util::bitcode::IsBitcode(_data + i)) {
      offsets.push_back(i);
    }
  }
  offsets.push_back(_size);
  return offsets;
}

const std::string &BitcodeContainer::GetPrefix() const {
  return _prefix;
}

void BitcodeContainer::SetPrefix(std::string prefix) {
  _prefix = std::move(prefix);
}

}  // namespace ebc
