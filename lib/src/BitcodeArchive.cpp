#include "BitcodeArchive.h"

extern "C" {
#include <xar/xar.h>
}

#include <cstdio>
#include <fstream>
#include <fstream>
#include <iostream>
#include <streambuf>

namespace ebc {

BitcodeArchive::BitcodeArchive(std::string name, const std::uint8_t *uuid, const char *data, std::uint32_t size)
    : _name(name), _uuid(), _data(nullptr), _size(size), _metadata(nullptr) {
  std::copy(uuid, uuid + 16, _uuid);
  _data = reinterpret_cast<char *>(std::malloc(size * sizeof(char)));
  std::copy(data, data + size, _data);
  _metadata = std::make_unique<BitcodeMetadata>(GetMetadataXml());
}

BitcodeArchive::BitcodeArchive(BitcodeArchive &&bitcodeArchive)
    : _name(bitcodeArchive._name)
    , _uuid()
    , _data(bitcodeArchive._data)
    , _size(bitcodeArchive._size)
    , _metadata(std::move(bitcodeArchive._metadata)) {
  std::copy(bitcodeArchive._uuid, bitcodeArchive._uuid + 16, _uuid);
  bitcodeArchive._data = nullptr;
}

BitcodeArchive::~BitcodeArchive() {
  delete _data;
}

std::string BitcodeArchive::GetName() const {
  return _name;
}

std::string BitcodeArchive::GetUUID() const {
  auto u = _uuid;
  char buf[256];
  sprintf(buf, "%2.2X%2.2X%2.2X%2.2X-%2.2X%2.2X-%2.2X%2.2X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X", u[0], u[1], u[2],
          u[3], u[4], u[5], u[6], u[7], u[8], u[9], u[10], u[11], u[12], u[13], u[14], u[15]);
  return std::string(buf);
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
        return files;
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
    auto clangCommands = _metadata->GetClangCommands(path);
    bitcodeFile.SetClangCommands(clangCommands);

    // Add to list of bitcode files
    files.push_back(bitcodeFile);

    free(path);
  }

  return files;
}

const BitcodeMetadata &BitcodeArchive::GetMetadata() const {
  return *_metadata;
}

std::string BitcodeArchive::GetMetadataXml() const {
  // Write archive to filesystem and read xar
  xar_t x;
  std::string xarFile = WriteXarToFile();
  x = xar_open(xarFile.c_str(), READ);
  std::remove(xarFile.c_str());

  // Write metadata to temporary file
  std::string metadataXmlFile = _name + "_metadata.xar";
  xar_serialize(x, metadataXmlFile.c_str());

  // Read Metadata to string and remove temporary file
  std::ifstream t(metadataXmlFile);
  std::string xml((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  std::remove(metadataXmlFile.c_str());

  return xml;
}
}
