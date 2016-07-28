#include "BitcodeRetriever.h"

#include "BitcodeArchive.h"
#include "BitcodeContainer.h"

#include "llvm/ADT/Triple.h"
#include "llvm/Object/ELFObjectFile.h"
#include "llvm/Object/MachO.h"
#include "llvm/Object/MachOUniversal.h"

#include <memory>
#include <string>

using namespace llvm;
using namespace llvm::object;

namespace ebc {

BitcodeRetriever::BitcodeRetriever(std::string objectPath) : _objectPath(objectPath) {}

std::vector<std::unique_ptr<BitcodeContainer>> BitcodeRetriever::GetBitcodeContainers() {
  auto bitcodeContainers = std::vector<std::unique_ptr<BitcodeContainer>>();
  auto binaryOrErr = createBinary(_objectPath);

  if (!binaryOrErr) {
    return bitcodeContainers;
  }

  auto &binary = *binaryOrErr.get().getBinary();
  if (MachOUniversalBinary *universalBinary = dyn_cast<MachOUniversalBinary>(&binary)) {
    for (auto object : universalBinary->objects()) {
      if (auto machOObject = object.getAsObjectFile()) {
        auto bitcodeArchive = GetBitcodeContainerFromMachO(machOObject->get());
        if (bitcodeArchive) {
          bitcodeContainers.push_back(std::move(bitcodeArchive));
        }
      }
    }
  } else if (auto machOObjectFile = dyn_cast<MachOObjectFile>(&binary)) {
    auto bitcodeArchive = GetBitcodeContainerFromMachO(machOObjectFile);
    if (bitcodeArchive) {
      bitcodeContainers.push_back(std::move(bitcodeArchive));
    }
  } else if (auto object = dyn_cast<ObjectFile>(&binary)) {
    auto bitcodeContainer = GetBitcodeContainerFromObject(object);
    if (bitcodeContainer) {
      bitcodeContainers.push_back(std::move(bitcodeContainer));
    }
  }

  return bitcodeContainers;
}

std::unique_ptr<BitcodeContainer> BitcodeRetriever::GetBitcodeContainerFromObject(
    llvm::object::ObjectFile *objectFile) const {
  BitcodeContainer *bitcodeContainer = nullptr;
  const std::string name = objectFile->getFileFormatName().str();
  std::vector<std::string> commands;
  for (const SectionRef &section : objectFile->sections()) {
    StringRef sectName;
    section.getName(sectName);

    if (sectName == ".llvmbc") {
      auto data = GetSectionData(section);

      bitcodeContainer = new BitcodeContainer(data.first, data.second);
      bitcodeContainer->SetName(name);
      bitcodeContainer->SetArch(TripleToArch(objectFile->getArch()));
    } else if (sectName == ".llvmcmd") {
      commands = GetCommands(section);
    }
  }
  AddIfNotNull(bitcodeContainer, commands);
  return std::unique_ptr<BitcodeContainer>(bitcodeContainer);
}

std::unique_ptr<BitcodeContainer> BitcodeRetriever::GetBitcodeContainerFromMachO(
    llvm::object::MachOObjectFile *machOObjectFile) const {
  BitcodeArchive *bitcodeArchive = nullptr;

  const std::string name = machOObjectFile->getFileFormatName().str();
  std::vector<std::string> commands;
  for (const SectionRef &section : machOObjectFile->sections()) {
    // Get section name
    DataRefImpl dataRef = section.getRawDataRefImpl();
    StringRef segName = machOObjectFile->getSectionFinalSegmentName(dataRef);

    if (segName == "__LLVM") {
      StringRef sectName;
      section.getName(sectName);
      if (sectName == "__bundle") {
        auto data = GetSectionData(section);

        bitcodeArchive = new BitcodeArchive(data.first, data.second);
        bitcodeArchive->SetName(name);
        bitcodeArchive->SetArch(TripleToArch(machOObjectFile->getArch()));
        bitcodeArchive->SetUuid(machOObjectFile->getUuid().data());
      } else if (sectName == "__cmd") {
        commands = GetCommands(section);
      }
    }
  }
  AddIfNotNull(bitcodeArchive, commands);
  return std::unique_ptr<BitcodeContainer>(bitcodeArchive);
}

std::pair<const char *, std::uint32_t> BitcodeRetriever::GetSectionData(const llvm::object::SectionRef &section) const {
  StringRef bytesStr;
  section.getContents(bytesStr);
  const char *sect = reinterpret_cast<const char *>(bytesStr.data());
  uint32_t sect_size = bytesStr.size();
  return std::make_pair(sect, sect_size);
}

std::vector<std::string> BitcodeRetriever::GetCommands(const llvm::object::SectionRef &section) const {
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

void BitcodeRetriever::AddIfNotNull(BitcodeContainer *bitcodeContainer, std::vector<std::string> commands) const {
  if (bitcodeContainer != nullptr) {
    bitcodeContainer->SetCommands(commands);
  }
}

std::string BitcodeRetriever::TripleToArch(unsigned arch) {
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
}
