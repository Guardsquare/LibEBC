#include "BitcodeRetriever.h"

#include "llvm/ADT/Triple.h"
#include "llvm/Object/MachO.h"
#include "llvm/Object/MachOUniversal.h"

#include <memory>
#include <string>

using namespace llvm;

namespace ebc {

BitcodeRetriever::BitcodeRetriever(std::string objectPath) : _objectPath(objectPath) {}

std::vector<std::unique_ptr<BitcodeArchive>> BitcodeRetriever::GetBitcodeArchives() {
  auto bitcodeArchives = std::vector<std::unique_ptr<BitcodeArchive>>();
  auto memoryBuffer = MemoryBuffer::getFile(_objectPath);
  if (!memoryBuffer) return bitcodeArchives;

  std::error_code error;

  // Try Universal MachO
  auto machOUniversalBinary = std::make_unique<object::MachOUniversalBinary>((*memoryBuffer)->getMemBufferRef(), error);
  if (!error) {
    for (auto object : machOUniversalBinary->objects()) {
      if (auto machOObject = object.getAsObjectFile()) {
        auto bitcodeArchive = GetBitcodeArchive(machOObject->get());
        if (bitcodeArchive) {
          bitcodeArchives.push_back(std::move(bitcodeArchive));
        }
      }
    }
    return bitcodeArchives;
  }

  // Try regular, thin MachO
  if (auto machOObject = object::ObjectFile::createMachOObjectFile((*memoryBuffer)->getMemBufferRef())) {
    auto bitcodeArchive = GetBitcodeArchive(machOObject->get());
    if (bitcodeArchive) {
      bitcodeArchives.push_back(std::move(bitcodeArchive));
    }
  }

  return bitcodeArchives;
}

std::unique_ptr<BitcodeArchive> BitcodeRetriever::GetBitcodeArchive(
    llvm::object::MachOObjectFile *machOObjectFile) const {
  std::string name = machOObjectFile->getFileFormatName().str();

  for (const object::SectionRef &section : machOObjectFile->sections()) {
    StringRef sectName;
    section.getName(sectName);

    object::DataRefImpl dataRef = section.getRawDataRefImpl();
    StringRef segName = machOObjectFile->getSectionFinalSegmentName(dataRef);

    if (segName == "__LLVM" && sectName == "__bundle") {
      StringRef bytesStr;
      section.getContents(bytesStr);
      const char *sect = reinterpret_cast<const char *>(bytesStr.data());
      uint32_t sect_size = bytesStr.size();

      auto bitcodeArchive = std::make_unique<BitcodeArchive>(sect, sect_size);
      bitcodeArchive->SetName(name);
      bitcodeArchive->SetArch(TripleToArch(machOObjectFile->getArch()));
      bitcodeArchive->SetUuid(machOObjectFile->getUuid().data());
      return bitcodeArchive;
    }
  }
  return nullptr;
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
