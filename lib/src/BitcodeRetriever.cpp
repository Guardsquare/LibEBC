#include "ebc/BitcodeRetriever.h"

#include "ebc/BitcodeArchive.h"
#include "ebc/BitcodeContainer.h"
#include "ebc/EbcError.h"

#include "llvm/ADT/Triple.h"
#include "llvm/Object/ELFObjectFile.h"
#include "llvm/Object/MachO.h"
#include "llvm/Object/MachOUniversal.h"

#include <algorithm>
#include <memory>
#include <string>

using namespace llvm;
using namespace llvm::object;

namespace ebc {

static std::string StripFileName(std::string fileName) {
  auto pos = fileName.rfind('/');
  return pos == std::string::npos ? fileName : fileName.substr(pos + 1);
}

BitcodeRetriever::BitcodeRetriever(std::string objectPath) : _objectPath(std::move(objectPath)) {}

std::vector<std::unique_ptr<BitcodeContainer>> BitcodeRetriever::GetBitcodeContainers() {
  auto binaryOrErr = createBinary(_objectPath);

  if (!binaryOrErr) {
    throw EbcError("Invalid binary");
  }

  auto &binary = *binaryOrErr.get().getBinary();
  return GetBitcodeContainers(binary);
}

std::vector<std::unique_ptr<BitcodeContainer>> BitcodeRetriever::GetBitcodeContainers(
    const llvm::object::Binary &binary) {
  auto bitcodeContainers = std::vector<std::unique_ptr<BitcodeContainer>>();

  if (const auto *universalBinary = dyn_cast<MachOUniversalBinary>(&binary)) {
    for (auto object : universalBinary->objects()) {
      if (auto machOObject = object.getAsObjectFile()) {
        auto container = GetBitcodeContainerFromMachO(machOObject->get());
        if (container) {
          bitcodeContainers.push_back(std::move(container));
        }
      } else if (const auto archive = object.getAsArchive()) {
        auto containers = GetBitcodeContainersFromArchive(*(archive.get()));
        bitcodeContainers.reserve(bitcodeContainers.size() + containers.size());
        std::move(std::begin(containers), std::end(containers), std::back_inserter(bitcodeContainers));
      } else {
        throw EbcError("Unrecognized MachO universal binary");
      }
    }
  } else if (const auto machOObjectFile = dyn_cast<MachOObjectFile>(&binary)) {
    auto container = GetBitcodeContainerFromMachO(machOObjectFile);
    if (container) {
      bitcodeContainers.push_back(std::move(container));
    }
  } else if (const auto object = dyn_cast<ObjectFile>(&binary)) {
    auto container = GetBitcodeContainerFromObject(object);
    if (container) {
      bitcodeContainers.push_back(std::move(container));
    }
  } else if (const auto archive = dyn_cast<Archive>(&binary)) {
    return GetBitcodeContainersFromArchive(*archive);
  } else {
    throw EbcError("Unsupported binary");
  }

  return bitcodeContainers;
}

std::vector<std::unique_ptr<BitcodeContainer>> BitcodeRetriever::GetBitcodeContainersFromArchive(
    const Archive &archive) {
  auto bitcodeContainers = std::vector<std::unique_ptr<BitcodeContainer>>();
  for (const auto &child : archive.children()) {
    const auto childOrErr = child->getAsBinary();
    if (childOrErr) {
      auto containers = GetBitcodeContainers(*(childOrErr.get()));
      bitcodeContainers.reserve(bitcodeContainers.size() + containers.size());
      std::move(std::begin(containers), std::end(containers), std::back_inserter(bitcodeContainers));
    }
  }
  return bitcodeContainers;
}

std::unique_ptr<BitcodeContainer> BitcodeRetriever::GetBitcodeContainerFromObject(
    const llvm::object::ObjectFile *objectFile) {
  BitcodeContainer *bitcodeContainer = nullptr;

  std::vector<std::string> commands;
  for (const SectionRef &section : objectFile->sections()) {
    StringRef sectName;
    section.getName(sectName);

    if (sectName == ".llvmbc") {
      auto data = GetSectionData(section);
      bitcodeContainer = new BitcodeContainer(data.first, data.second);
    } else if (sectName == ".llvmcmd") {
      commands = GetCommands(section);
    }
  }

  if (bitcodeContainer == nullptr) {
    throw EbcError("No bitcode section in " + objectFile->getFileName().str());
  }

  bitcodeContainer->SetCommands(commands);
  bitcodeContainer->GetBinaryMetadata().SetFileName(StripFileName(objectFile->getFileName()));
  bitcodeContainer->GetBinaryMetadata().SetFileFormatName(objectFile->getFileFormatName());
  bitcodeContainer->GetBinaryMetadata().SetArch(
      llvm::Triple::getArchTypeName(static_cast<Triple::ArchType>(objectFile->getArch())));

  return std::unique_ptr<BitcodeContainer>(bitcodeContainer);
}

std::unique_ptr<BitcodeContainer> BitcodeRetriever::GetBitcodeContainerFromMachO(
    const llvm::object::MachOObjectFile *objectFile) {
  BitcodeContainer *bitcodeContainer = nullptr;

  const std::string name = objectFile->getFileFormatName().str();
  std::vector<std::string> commands;
  for (const SectionRef &section : objectFile->sections()) {
    // Get section name
    DataRefImpl dataRef = section.getRawDataRefImpl();
    StringRef segName = objectFile->getSectionFinalSegmentName(dataRef);

    if (segName == "__LLVM") {
      StringRef sectName;
      section.getName(sectName);
      if (sectName == "__bundle") {
        // Embedded bitcode in universal binary.
        auto data = GetSectionData(section);
        bitcodeContainer = new BitcodeArchive(data.first, data.second);
      } else if (sectName == "__bitcode") {
        // Embedded bitcode in single MachO object.
        auto data = GetSectionData(section);
        bitcodeContainer = new BitcodeContainer(data.first, data.second);
      } else if (sectName == "__cmd" || sectName == "__cmdline") {
        commands = GetCommands(section);
      }
    }
  }

  if (bitcodeContainer == nullptr) {
    throw EbcError("No bitcode section in " + objectFile->getFileName().str());
  }

  bitcodeContainer->SetCommands(commands);
  bitcodeContainer->GetBinaryMetadata().SetFileName(StripFileName(objectFile->getFileName()));
  bitcodeContainer->GetBinaryMetadata().SetFileFormatName(objectFile->getFileFormatName());
  bitcodeContainer->GetBinaryMetadata().SetArch(
      llvm::Triple::getArchTypeName(static_cast<Triple::ArchType>(objectFile->getArch())));
  bitcodeContainer->GetBinaryMetadata().SetUuid(objectFile->getUuid().data());

  return std::unique_ptr<BitcodeContainer>(bitcodeContainer);
}

std::pair<const char *, std::uint32_t> BitcodeRetriever::GetSectionData(const llvm::object::SectionRef &section) {
  StringRef bytesStr;
  section.getContents(bytesStr);
  const char *sect = reinterpret_cast<const char *>(bytesStr.data());
  uint32_t sect_size = bytesStr.size();
  return std::make_pair(sect, sect_size);
}

std::vector<std::string> BitcodeRetriever::GetCommands(const llvm::object::SectionRef &section) {
  auto data = GetSectionData(section);
  const char *p = data.first;
  const char *end = data.first + data.second;

  std::vector<std::string> cmds;
  do {
    cmds.push_back(std::string(p));
    p += cmds.back().size() + 1;
  } while (p < end);

  return cmds;
}

}  // namespace ebc
