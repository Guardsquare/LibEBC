#include "ebc/BitcodeRetriever.h"

#include "ebc/BitcodeArchive.h"
#include "ebc/BitcodeContainer.h"

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

static std::string TripleToArch(unsigned arch) {
  switch (arch) {
    case Triple::x86:
      return "x86";
    case Triple::x86_64:
      return "x86_64";
    case Triple::arm:
      return "arm";
    case Triple::aarch64:
      return "arm64";
  }
  return "unknown";
}

static std::string StripFileName(std::string fileName) {
  auto pos = fileName.rfind('/');
  return pos == std::string::npos ? fileName : fileName.substr(pos + 1);
}

BitcodeRetriever::BitcodeRetriever(std::string objectPath) : _objectPath(std::move(objectPath)) {}

std::vector<std::unique_ptr<BitcodeContainer>> BitcodeRetriever::GetBitcodeContainers() {
  auto binaryOrErr = createBinary(_objectPath);

  if (!binaryOrErr) {
    return {};
  }

  auto &binary = *binaryOrErr.get().getBinary();
  return GetBitcodeContainers(binary);
}

std::vector<std::unique_ptr<BitcodeContainer>> BitcodeRetriever::GetBitcodeContainers(llvm::object::Binary &binary) {
  auto bitcodeContainers = std::vector<std::unique_ptr<BitcodeContainer>>();

  if (MachOUniversalBinary *universalBinary = dyn_cast<MachOUniversalBinary>(&binary)) {
    for (auto object : universalBinary->objects()) {
      if (auto machOObject = object.getAsObjectFile()) {
        auto bitcodeContainer = GetBitcodeContainerFromMachO(machOObject->get());
        if (bitcodeContainer) {
          bitcodeContainers.push_back(std::move(bitcodeContainer));
        }
      } else if (auto archive = object.getAsArchive()) {
        for (auto &child : (*archive)->children()) {
          auto childOrErr = child->getAsBinary();
          if (childOrErr) {
            auto archiveContainers = GetBitcodeContainers(*(childOrErr.get()));
            bitcodeContainers.reserve(bitcodeContainers.size() + archiveContainers.size());
            std::move(std::begin(archiveContainers), std::end(archiveContainers),
                      std::back_inserter(bitcodeContainers));
          }
        }
      }
    }
  } else if (auto machOObjectFile = dyn_cast<MachOObjectFile>(&binary)) {
    auto bitcodeContainer = GetBitcodeContainerFromMachO(machOObjectFile);
    if (bitcodeContainer) {
      bitcodeContainers.push_back(std::move(bitcodeContainer));
    }
  } else if (auto object = dyn_cast<ObjectFile>(&binary)) {
    auto bitcodeContainer = GetBitcodeContainerFromObject(object);
    if (bitcodeContainer) {
      bitcodeContainers.push_back(std::move(bitcodeContainer));
    }
  } else if (auto archive = dyn_cast<Archive>(&binary)) {
    return GetBitcodeContainersFromArchive(*archive);
  }

  return bitcodeContainers;
}

std::vector<std::unique_ptr<BitcodeContainer>> BitcodeRetriever::GetBitcodeContainersFromArchive(Archive &archive) {
  for (auto &child : archive.children()) {
    auto childOrErr = child->getAsBinary();
    if (childOrErr) {
      return GetBitcodeContainers(*(childOrErr.get()));
    }
  }
  return {};
}

std::unique_ptr<BitcodeContainer> BitcodeRetriever::GetBitcodeContainerFromObject(
    llvm::object::ObjectFile *objectFile) {
  BitcodeContainer *bitcodeContainer = nullptr;
  const std::string name = objectFile->getFileFormatName().str();
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

  if (bitcodeContainer != nullptr) {
    bitcodeContainer->SetCommands(commands);
    bitcodeContainer->GetBinaryMetadata().SetFileName(StripFileName(objectFile->getFileName()));
    bitcodeContainer->GetBinaryMetadata().SetFileFormatName(objectFile->getFileFormatName());
    bitcodeContainer->GetBinaryMetadata().SetArch(TripleToArch(objectFile->getArch()));
  }

  return std::unique_ptr<BitcodeContainer>(bitcodeContainer);
}

std::unique_ptr<BitcodeContainer> BitcodeRetriever::GetBitcodeContainerFromMachO(
    llvm::object::MachOObjectFile *objectFile) {
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

  if (bitcodeContainer != nullptr) {
    bitcodeContainer->SetCommands(commands);
    bitcodeContainer->GetBinaryMetadata().SetFileName(StripFileName(objectFile->getFileName()));
    bitcodeContainer->GetBinaryMetadata().SetFileFormatName(objectFile->getFileFormatName());
    bitcodeContainer->GetBinaryMetadata().SetArch(TripleToArch(objectFile->getArch()));
    bitcodeContainer->GetBinaryMetadata().SetUuid(objectFile->getUuid().data());
  }

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
