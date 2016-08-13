#include "ebc/BitcodeContainer.h"
#include "ebc/BitcodeUtil.h"

#include <algorithm>
#include <cstdio>
#include <streambuf>

namespace ebc {

BitcodeContainer::BitcodeContainer(const char *data, std::uint32_t size) : _data(nullptr), _size(size), _commands() {
  SetData(data, size);
}

BitcodeContainer::BitcodeContainer(BitcodeContainer &&bitcodeContainer)
    : _data(nullptr)
    , _size(bitcodeContainer._size)
    , _commands(bitcodeContainer._commands)
    , _binaryMetadata(bitcodeContainer._binaryMetadata) {
  SetData(bitcodeContainer._data, bitcodeContainer._size);
  bitcodeContainer._data = nullptr;
}

BitcodeContainer::~BitcodeContainer() {
  if (_data != nullptr) {
    delete _data;
    _data = nullptr;
  }
}

bool BitcodeContainer::IsArchive() const {
  return false;
}

const std::vector<std::string> &BitcodeContainer::GetCommands() const {
  return _commands;
}

void BitcodeContainer::SetCommands(const std::vector<std::string> &commands) {
  _commands = commands;
}

void BitcodeContainer::SetData(const char *data, std::uint32_t size) {
  if (size > 0) {
    _data = reinterpret_cast<char *>(std::malloc(size * sizeof(char)));
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

std::vector<BitcodeFile> BitcodeContainer::GetBitcodeFiles(std::string prefix) const {
  std::vector<BitcodeFile> files;

  if (_size < 4) {
    return files;
  }

  auto offsets = GetBitcodeFileOffsets();
  for (std::uint32_t i = 0; i < offsets.size() - 1; ++i) {
    auto begin = offsets[i];
    auto end = offsets[i + 1];
    auto size = end - begin;
    auto fileName = util::MakeBitcodeFileName(prefix, _binaryMetadata.GetFileFormatName(), i);
    util::WriteBitcodeFile(_data + begin, size, fileName);

    BitcodeFile bitcodeFile(fileName);
    bitcodeFile.SetCommands(_commands);
    files.push_back(bitcodeFile);
  }

  return files;
}

std::vector<std::uint32_t> BitcodeContainer::GetBitcodeFileOffsets() const {
  std::vector<std::uint32_t> offsets;
  for (std::uint32_t i = 0; i < _size - 3; ++i) {
    if (util::IsBitcodeFile(_data + i)) {
      offsets.push_back(i);
    }
  }
  offsets.push_back(_size);
  return offsets;
}
}
