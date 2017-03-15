#include "ebc/BitcodeArchive.h"

#include "ebc/BitcodeMetadata.h"
#include "ebc/Config.h"
#include "ebc/EbcError.h"
#include "ebc/EmbeddedFile.h"
#include "ebc/EmbeddedFileFactory.h"

#include "ebc/util/Bitcode.h"
#include "ebc/util/UUID.h"
#include "ebc/util/Xar.h"

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
#include <utility>

using namespace ebc::util;

namespace ebc {

BitcodeArchive::BitcodeArchive(const char *data, std::uint32_t size)
    : BitcodeContainer(data, size), _metadata(nullptr) {
  SetMetadata();
}

BitcodeArchive::BitcodeArchive(BitcodeArchive &&bitcodeArchive) noexcept : BitcodeContainer(std::move(bitcodeArchive)) {
  SetMetadata();
}

std::unique_ptr<BitcodeContainer> BitcodeArchive::BitcodeArchiveFromFile(std::string path) {
  if (path.empty()) {
    return nullptr;
  }

  std::ifstream input(path, std::ifstream::binary);

  if (!input) {
    std::cerr << "Unable to open " << path << std::endl;
    return nullptr;
  }

  std::vector<char> buffer((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  return std::unique_ptr<BitcodeContainer>(new BitcodeArchive(&buffer[0], buffer.size()));
}

bool BitcodeArchive::IsArchive() const {
  return true;
}

void BitcodeArchive::SetMetadata() noexcept {
  _metadata = std::make_unique<BitcodeMetadata>(GetMetadataXml());
}

std::string BitcodeArchive::WriteXarToFile() const {
  const std::string fileName = GetPrefix() + util::uuid::UuidToString(util::uuid::GenerateUUID()) + ".xar";
  auto data = GetData();
  bitcode::WriteToFile(data.first, data.second, fileName);
  return fileName;
}

std::vector<std::unique_ptr<EmbeddedFile>> BitcodeArchive::GetEmbeddedFiles() const {
  if (IsEmpty()) return {};

  // Write xar to disk.
  const std::string archive = WriteXarToFile();

  // Create bitcode files from archive.
  auto embeddedFiles = std::vector<std::unique_ptr<EmbeddedFile>>();

  for (auto filePair : xar::Extract(archive, GetPrefix())) {
    std::string path = filePair.first;
    std::string file = filePair.second;

    auto fileType = _metadata->GetFileType(path);
    auto embeddedFile = EmbeddedFileFactory::CreateEmbeddedFile(file, fileType);

    if (!embeddedFile) continue;

    // Add clang commands.
    auto clangCommands = _metadata->GetClangCommands(path);
    embeddedFile->SetCommands(clangCommands, EmbeddedFile::CommandSource::Clang);

    // Add swift commands.
    auto swiftCommands = _metadata->GetSwiftCommands(path);
    embeddedFile->SetCommands(swiftCommands, EmbeddedFile::CommandSource::Swift);

    // Add to list of bitcode files.
    embeddedFiles.push_back(std::move(embeddedFile));
  }

  // Remove xar again.
  std::remove(archive.c_str());

  return embeddedFiles;
}

const BitcodeMetadata &BitcodeArchive::GetMetadata() const {
  return *_metadata;
}

std::string BitcodeArchive::GetMetadataXml() const noexcept {
  auto data = GetData();

  if (data.first == nullptr) {
    return {};
  }

  const std::string xarFile = WriteXarToFile();
  const std::string xmlFile = GetPrefix() + util::uuid::UuidToString(util::uuid::GenerateUUID()) + ".xml";

  if (xar::WriteTOC(xarFile, xmlFile)) {
    // Read Metadata in memory.
    std::ifstream t(xmlFile);
    std::string xml((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

    // Remove temporary files.
    std::remove(xarFile.c_str());
    std::remove(xmlFile.c_str());

    return xml;
  }

  return {};
}

}  // namespace ebc
