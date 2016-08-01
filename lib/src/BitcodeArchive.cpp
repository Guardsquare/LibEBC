#include "ebc/BitcodeArchive.h"

#include "ebc/BitcodeMetadata.h"
#include "ebc/Config.h"

#ifdef HAVE_LIBXAR
extern "C" {
#include <xar/xar.h>
}
#endif

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <streambuf>

namespace ebc {

BitcodeArchive::BitcodeArchive(const char *data, std::uint32_t size)
    : BitcodeContainer(data, size), _metadata(nullptr) {
  SetMetadata();
}

BitcodeArchive::BitcodeArchive(BitcodeArchive &&bitcodeArchive) : BitcodeContainer(std::move(bitcodeArchive)) {
  SetMetadata();
}

bool BitcodeArchive::IsArchive() const {
  return true;
}

void BitcodeArchive::SetMetadata() {
  _metadata = std::make_unique<BitcodeMetadata>(GetMetadataXml());
}

std::string BitcodeArchive::WriteXarToFile(std::string fileName) const {
  if (fileName.empty()) {
    fileName = GetName() + ".xar";
  }

  auto data = GetData();
  std::ofstream outfile(fileName, std::ofstream::binary);
  outfile.write(data.first, data.second);
  outfile.close();
  return fileName;
}

std::vector<BitcodeFile> BitcodeArchive::GetBitcodeFiles() const {
  auto files = std::vector<BitcodeFile>();

#ifdef HAVE_LIBXAR
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

    if (std::strcmp(type, "file") != 0) {
      free(path);
      continue;
    }

    if (xar_extract_tostream_init(x, xf, &xs) != XAR_STREAM_OK) {
      std::cerr << "Error initializing stream" << std::endl;
      free(path);
      continue;
    }

    // Write bitcode to file
    auto filePath = GetName() + "_" + std::to_string(i++) + ".bc";
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
    bitcodeFile.SetCommands(clangCommands);

    // Add swift commands
    auto swiftCommands = _metadata->GetSwiftCommands(path);
    bitcodeFile.SetCommands(swiftCommands);

    // Add to list of bitcode files
    files.push_back(bitcodeFile);

    free(path);
  }
  xar_iter_free(xi);
  xar_close(x);
#endif

  return files;
}

const BitcodeMetadata &BitcodeArchive::GetMetadata() const {
  return *_metadata;
}

std::string BitcodeArchive::GetMetadataXml() const {
  auto data = GetData();
  if (data.first == nullptr) {
    return std::string();
  }

  std::string xarFile = WriteXarToFile();
  std::string metadataXmlFile = GetName() + "_metadata.xar";

#ifdef HAVE_LIBXAR
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
#else
  return std::string();
#endif
}
}
