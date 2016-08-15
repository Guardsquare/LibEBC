#include "ebc/BitcodeArchive.h"

#include "ebc/BitcodeFile.h"
#include "ebc/BitcodeMetadata.h"
#include "ebc/Config.h"
#include "ebc/EbcError.h"
#include "ebc/util/Bitcode.h"
#include "ebc/util/Namer.h"

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

BitcodeArchive::BitcodeArchive(BitcodeArchive &&bitcodeArchive) noexcept : BitcodeContainer(std::move(bitcodeArchive)) {
  SetMetadata();
}

bool BitcodeArchive::IsArchive() const {
  return true;
}

void BitcodeArchive::SetMetadata() noexcept {
  _metadata = std::make_unique<BitcodeMetadata>(GetMetadataXml());
}

std::string BitcodeArchive::WriteXarToFile(std::string fileName) const {
  if (fileName.empty()) {
    fileName = GetBinaryMetadata().GetFileName() + ".xar";
  }

  auto data = GetData();
  util::bitcode::WriteFile(data.first, data.second, fileName);
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
  if (x == nullptr) {
    throw EbcError("Could not open xar archive");
  }

  xi = xar_iter_new();
  if (xi == nullptr) {
    xar_close(x);
    throw EbcError("Could not read xar archive");
  }

  for (xf = xar_file_first(x, xi); xf != nullptr; xf = xar_file_next(xi)) {
    char *path = xar_get_path(xf);
    const char *type;
    xar_prop_get(xf, "type", &type);

    if (type == nullptr) {
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
    auto fileName = util::Namer::GetFileName();
    std::FILE *output = std::fopen(fileName.c_str(), "wb");
    if (output == nullptr) {
      std::cerr << "Error opening output file" << std::endl;
      continue;
    }

    xs.avail_out = sizeof(buffer);
    xs.next_out = buffer;

    int32_t ret;
    while ((ret = xar_extract_tostream(&xs)) != XAR_STREAM_END) {
      if (ret == XAR_STREAM_ERR) {
        std::cerr << "Error extracting stream" << std::endl;
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
    auto bitcodeFile = BitcodeFile(fileName);

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

std::string BitcodeArchive::GetMetadataXml() const noexcept {
  auto data = GetData();
  if (data.first == nullptr) {
    return {};
  }

  std::string xarFile = WriteXarToFile();
  std::string metadataXmlFile = GetBinaryMetadata().GetFileFormatName() + "_metadata.xar";

#ifdef HAVE_LIBXAR
  // Write archive to filesystem and read xar
  xar_t x = xar_open(xarFile.c_str(), READ);
  if (x == nullptr) {
    return {};
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

}  // namespace ebc
