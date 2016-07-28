#include "BitcodeContainer.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <streambuf>

namespace ebc {

BitcodeContainer::BitcodeContainer(const char *data, std::uint32_t size)
    : _name(), _cmd(), _arch(), _uuid(), _data(nullptr), _size(size) {
  SetData(data, size);
}

BitcodeContainer::BitcodeContainer(BitcodeContainer &&bitcodeContainer)
    : _name(bitcodeContainer._name)
    , _cmd(bitcodeContainer._cmd)
    , _arch(bitcodeContainer._arch)
    , _uuid(bitcodeContainer._uuid)
    , _data(nullptr)
    , _size(bitcodeContainer._size) {
  SetData(bitcodeContainer._data, bitcodeContainer._size);
  bitcodeContainer._data = nullptr;
}

BitcodeContainer::~BitcodeContainer() {
  if (_data != nullptr) {
    delete _data;
    _data = nullptr;
  }
}

std::string BitcodeContainer::GetName() const {
  return _name;
}

void BitcodeContainer::SetName(std::string name) {
  _name = name;
}

std::string BitcodeContainer::GetCmd() const {
  return _cmd;
}

void BitcodeContainer::SetCmd(std::string cmd) {
  _cmd = cmd;
}

std::string BitcodeContainer::GetArch() const {
  return _arch;
}

void BitcodeContainer::SetArch(std::string arch) {
  _arch = arch;
}

void BitcodeContainer::SetUuid(const std::uint8_t *uuid) {
  if (uuid != nullptr) {
    std::copy_n(uuid, _uuid.size(), _uuid.begin());
  }
}

std::string BitcodeContainer::GetUUID() const {
  char buffer[UUID_ASCII_LENGTH + 1];
  sprintf(buffer, "%2.2X%2.2X%2.2X%2.2X-%2.2X%2.2X-%2.2X%2.2X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X", _uuid[0],
          _uuid[1], _uuid[2], _uuid[3], _uuid[4], _uuid[5], _uuid[6], _uuid[7], _uuid[8], _uuid[9], _uuid[10],
          _uuid[11], _uuid[12], _uuid[13], _uuid[14], _uuid[15]);
  return std::string(buffer);
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

std::vector<BitcodeFile> BitcodeContainer::GetBitcodeFiles() const {
  auto files = std::vector<BitcodeFile>();
  // FIXME
  return files;
}
}
