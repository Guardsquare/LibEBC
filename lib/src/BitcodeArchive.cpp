#include "BitcodeArchive.h"

#include "BitcodeMetadata.h"

extern "C" {
#include <xar/xar.h>
}

#include <cstdio>
#include <fstream>
#include <iostream>
#include <streambuf>

namespace ebc {

BitcodeArchive::BitcodeArchive(const char *data, std::uint32_t size)
    : _name(), _arch(), _uuid(), _data(nullptr), _size(size), _metadata(nullptr) {
  SetData(data, size);
  SetMetadata();
}

BitcodeArchive::BitcodeArchive(BitcodeArchive &&bitcodeArchive)
    : _name(bitcodeArchive._name)
    , _arch(bitcodeArchive._arch)
    , _uuid(bitcodeArchive._uuid)
    , _data(nullptr)
    , _size(bitcodeArchive._size)
    , _metadata(std::move(bitcodeArchive._metadata)) {
  SetData(bitcodeArchive._data, bitcodeArchive._size);
  SetMetadata();
  bitcodeArchive._data = nullptr;
}

BitcodeArchive::~BitcodeArchive() {
  if (_data != nullptr) {
    delete _data;
    _data = nullptr;
  }
}

std::string BitcodeArchive::GetName() const {
  return _name;
}

void BitcodeArchive::SetName(std::string name) {
  _name = name;
}

std::string BitcodeArchive::GetArch() const {
  return _arch;
}

void BitcodeArchive::SetArch(std::string arch) {
  _arch = arch;
}

void BitcodeArchive::SetUuid(const std::uint8_t *uuid) {
  if (uuid != nullptr) {
    std::copy_n(uuid, _uuid.size(), _uuid.begin());
  }
}

void BitcodeArchive::SetData(const char *data, std::uint32_t size) {
  if (size > 0) {
    _data = reinterpret_cast<char *>(std::malloc(size * sizeof(char)));
    std::copy(data, data + size, _data);
  }
}

void BitcodeArchive::SetMetadata() {
  _metadata = std::make_unique<BitcodeMetadata>(GetMetadataXml());
}

std::string BitcodeArchive::GetUUID() const {
  char buffer[UUID_ASCII_LENGTH + 1];
  sprintf(buffer, "%2.2X%2.2X%2.2X%2.2X-%2.2X%2.2X-%2.2X%2.2X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X", _uuid[0],
          _uuid[1], _uuid[2], _uuid[3], _uuid[4], _uuid[5], _uuid[6], _uuid[7], _uuid[8], _uuid[9], _uuid[10],
          _uuid[11], _uuid[12], _uuid[13], _uuid[14], _uuid[15]);
  return std::string(buffer);
}

std::string BitcodeArchive::WriteXarToFile(std::string fileName) const {
  if (fileName.empty()) {
    fileName = _name + ".xar";
  }

  std::ofstream outfile(fileName, std::ofstream::binary);
  outfile.write(_data, _size);
  outfile.close();
  return fileName;
}

std::vector<BitcodeFile> BitcodeArchive::GetBitcodeFiles() const {
  auto files = std::vector<BitcodeFile>();

  xar_t x;
  xar_iter_t xi;
  xar_file_t xf;
  xar_stream xs;
  char buffer[8192];

  auto archivePath = WriteXarToFile();

  x = xar_open(archivePath.c_str(), READ);
  if (!x) {
    std::cerr << "Error opening archive" << std::endl;
    return files;
  }

  xi = xar_iter_new();
  if (!xi) {
    std::cerr << "Error creating xar iterator" << std::endl;
    xar_close(x);
    return files;
  }

  int i = 0;
  for (xf = xar_file_first(x, xi); xf; xf = xar_file_next(xi)) {
    char *path = xar_get_path(xf);
    const char *type;
    xar_prop_get(xf, "type", &type);

    if (!type) {
      std::cerr << "File has no type" << std::endl;
      free(path);
      continue;
    }

    if (strcmp(type, "file") != 0) {
      free(path);
      continue;
    }

    if (xar_extract_tostream_init(x, xf, &xs) != XAR_STREAM_OK) {
      std::cerr << "Error initializing stream" << std::endl;
      free(path);
      continue;
    }

    // Write bitcode to file
    auto filePath = _name + "_" + std::to_string(i++) + ".bc";
    std::FILE *output = std::fopen(filePath.c_str(), "wb");
    if (!output) {
      std::cerr << "Error opening output file" << std::endl;
      continue;
    }

    xs.avail_out = sizeof(buffer);
    xs.next_out = buffer;

    int32_t ret;
    while ((ret = xar_extract_tostream(&xs)) != XAR_STREAM_END) {
      if (ret == XAR_STREAM_ERR) {
        std::cerr << "Error extracting stream" << std::endl;
        free(path);
        break;
      }
      std::fwrite(buffer, sizeof(char), sizeof(buffer) - xs.avail_out, output);

      xs.avail_out = sizeof(buffer);
      xs.next_out = buffer;
    }

    if (xar_extract_tostream_end(&xs) != XAR_STREAM_OK) {
      std::cerr << "Error ending stream" << std::endl;
    }

    std::fclose(output);

    // Create bitcode file
    auto bitcodeFile = BitcodeFile(filePath);

    // Add clang commands
    auto clangCommands = _metadata->GetClangCommands(path);
    bitcodeFile.SetClangCommands(clangCommands);

    // Add swift commands
    auto swiftCommands = _metadata->GetSwiftCommands(path);
    bitcodeFile.SetSwiftCommands(swiftCommands);

    // Add to list of bitcode files
    files.push_back(bitcodeFile);

    free(path);
  }
  xar_iter_free(xi);
  xar_close(x);

  return files;
}

const BitcodeMetadata &BitcodeArchive::GetMetadata() const {
  return *_metadata;
}

std::string BitcodeArchive::GetMetadataXml() const {
  if (_data == nullptr) {
    return std::string();
  }

  std::string xarFile = WriteXarToFile();
  std::string metadataXmlFile = _name + "_metadata.xar";

  // Write archive to filesystem and read xar
  xar_t x = xar_open(xarFile.c_str(), READ);
  if (x == nullptr) {
    return std::string();
  }

  xar_serialize(x, metadataXmlFile.c_str());
  xar_close(x);
  std::remove(xarFile.c_str());

  // Read Metadata to string and remove temporary file
  std::ifstream t(metadataXmlFile);
  std::string xml((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  std::remove(metadataXmlFile.c_str());

  return xml;
}
}
