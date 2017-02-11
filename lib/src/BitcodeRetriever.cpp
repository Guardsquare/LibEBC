#include "ebc/BitcodeRetriever.h"

#include "ebc/BitcodeArchive.h"
#include "ebc/BitcodeContainer.h"
#include "ebc/EbcError.h"

#include "llvm/ADT/Triple.h"
#include "llvm/Object/ELFObjectFile.h"
#include "llvm/Object/MachO.h"
#include "llvm/Object/MachOUniversal.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>

using namespace llvm;
using namespace llvm::object;

namespace ebc {

/// Strip path from file name.
static std::string StripFileName(std::string fileName) {
  auto pos = fileName.rfind('/');
  return pos == std::string::npos ? fileName : fileName.substr(pos + 1);
}

BitcodeRetriever::BitcodeRetriever(std::string objectPath) : _objectPath(std::move(objectPath)) {}

void BitcodeRetriever::SetArch(std::string arch) {
  _arch = arch;
}

std::vector<std::unique_ptr<BitcodeContainer>> BitcodeRetriever::GetBitcodeContainers() {
  auto binaryOrErr = createBinary(_objectPath);

  if (binaryOrErr) {
    auto &binary = *binaryOrErr.get().getBinary();
    return GetBitcodeContainers(binary);
  } else {
    llvm::consumeError(binaryOrErr.takeError());
    throw EbcError("Could not create binary from " + _objectPath);
  }

  return {};
}

std::vector<std::unique_ptr<BitcodeContainer>> BitcodeRetriever::GetBitcodeContainers(
    const llvm::object::Binary &binary) const {
  auto bitcodeContainers = std::vector<std::unique_ptr<BitcodeContainer>>();

  if (const auto *universalBinary = dyn_cast<MachOUniversalBinary>(&binary)) {
    // Iterate over all objects (i.e. architectures):
    for (auto object : universalBinary->objects()) {
      Expected<std::unique_ptr<MachOObjectFile>> machOObject = object.getAsObjectFile();
      if (machOObject) {
        auto container = GetBitcodeContainerFromMachO(machOObject->get());
        if (container) {
          bitcodeContainers.push_back(std::move(container));
        }
        continue;
      } else {
        llvm::consumeError(machOObject.takeError());
      }

      Expected<std::unique_ptr<Archive>> archive = object.getAsArchive();
      if (archive) {
        auto containers = GetBitcodeContainersFromArchive(*archive->get());
        // We have to move all containers so we can continue with the next architecture.
        bitcodeContainers.reserve(bitcodeContainers.size() + containers.size());
        std::copy_if(std::make_move_iterator(containers.begin()), std::make_move_iterator(containers.end()),
                     std::back_inserter(bitcodeContainers), [](const auto &uniquePtr) { return uniquePtr != nullptr; });
        continue;
      } else {
        llvm::consumeError(archive.takeError());
      }

      throw EbcError("Unrecognized MachO universal binary");
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
    // We can return early to prevent moving all containers in the vector.
    return GetBitcodeContainersFromArchive(*archive);
  } else {
    throw EbcError("Unsupported binary");
  }

  return bitcodeContainers;
}

std::vector<std::unique_ptr<BitcodeContainer>> BitcodeRetriever::GetBitcodeContainersFromArchive(
    const Archive &archive) const {
  Error err;
  auto children = archive.children(err);

  if (err) {
    throw EbcError("Couldn't get children from archive " + archive.getFileName().str());
  }

  auto bitcodeContainers = std::vector<std::unique_ptr<BitcodeContainer>>();
  for (const auto &child : children) {
    auto childOrErr = child.getAsBinary();
    if (childOrErr) {
      auto containers = GetBitcodeContainers(*(childOrErr.get()));
      bitcodeContainers.reserve(bitcodeContainers.size() + containers.size());
      std::move(std::begin(containers), std::end(containers), std::back_inserter(bitcodeContainers));
    }
  }
  return bitcodeContainers;
}

std::unique_ptr<BitcodeContainer> BitcodeRetriever::GetBitcodeContainerFromObject(
    const llvm::object::ObjectFile *objectFile) const {
  const auto arch = llvm::Triple::getArchTypeName(static_cast<Triple::ArchType>(objectFile->getArch()));
  if (!processArch(arch)) {
    return {};
  }

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
    throw NoBitcodeError("No bitcode section in " + objectFile->getFileName().str());
  }

  // Set commands
  bitcodeContainer->SetCommands(commands);

  // Set binary metadata
  bitcodeContainer->GetBinaryMetadata().SetFileName(StripFileName(objectFile->getFileName()));
  bitcodeContainer->GetBinaryMetadata().SetFileFormatName(objectFile->getFileFormatName());
  bitcodeContainer->GetBinaryMetadata().SetArch(arch);

  return std::unique_ptr<BitcodeContainer>(bitcodeContainer);
}

std::unique_ptr<BitcodeContainer> BitcodeRetriever::GetBitcodeContainerFromMachO(
    const llvm::object::MachOObjectFile *objectFile) const {
  // For MachO return the correct arch tripple.
  const std::string arch = objectFile->getArchTriple(nullptr).getArchName();
  if (!processArch(arch)) {
    return {};
  }

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
    throw NoBitcodeError("No bitcode section in " + objectFile->getFileName().str());
  }

  // Set commands
  bitcodeContainer->SetCommands(commands);

  // Set binary metadata
  bitcodeContainer->GetBinaryMetadata().SetFileName(StripFileName(objectFile->getFileName()));
  bitcodeContainer->GetBinaryMetadata().SetFileFormatName(objectFile->getFileFormatName());
  bitcodeContainer->GetBinaryMetadata().SetArch(arch);
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

  // Create list of strings from commands separated by null bytes.
  std::vector<std::string> cmds;
  do {
    // Creating a string from p consumes data until next null byte.
    cmds.push_back(std::string(p));
    // Continue after the null byte.
    p += cmds.back().size() + 1;
  } while (p < end);

  return cmds;
}

bool BitcodeRetriever::processArch(std::string arch) const {
  return _arch.empty() ? true : (_arch == arch);
}

}  // namespace ebc
