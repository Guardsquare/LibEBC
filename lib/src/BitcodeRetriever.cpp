#include "BitcodeRetriever.h"

#include "BitcodeArchive.h"

#include "llvm/ADT/Triple.h"
#include "llvm/Object/MachO.h"
#include "llvm/Object/MachOUniversal.h"

#include <memory>
#include <string>

using namespace llvm;
using namespace llvm::object;

namespace ebc {

BitcodeRetriever::BitcodeRetriever(std::string objectPath) : _objectPath(objectPath) {}

std::vector<std::unique_ptr<BitcodeArchive>> BitcodeRetriever::GetBitcodeArchives() {
  auto bitcodeArchives = std::vector<std::unique_ptr<BitcodeArchive>>();
  auto binaryOrErr = createBinary(_objectPath);

  if (!binaryOrErr) {
    return bitcodeArchives;
  }

  auto &binary = *binaryOrErr.get().getBinary();
  if (MachOUniversalBinary *universalBinary = dyn_cast<MachOUniversalBinary>(&binary)) {
    for (auto object : universalBinary->objects()) {
      if (auto machOObject = object.getAsObjectFile()) {
        auto bitcodeArchive = GetBitcodeArchive(machOObject->get());
        if (bitcodeArchive) {
          bitcodeArchives.push_back(std::move(bitcodeArchive));
        }
      }
    }
  } else if (auto machOObjectFile = dyn_cast<MachOObjectFile>(&binary)) {
    auto bitcodeArchive = GetBitcodeArchive(machOObjectFile);
    if (bitcodeArchive) {
      bitcodeArchives.push_back(std::move(bitcodeArchive));
    }
  }
  return bitcodeArchives;
}

std::unique_ptr<BitcodeArchive> BitcodeRetriever::GetBitcodeArchive(
    llvm::object::MachOObjectFile *machOObjectFile) const {
  std::string name = machOObjectFile->getFileFormatName().str();

  for (const SectionRef &section : machOObjectFile->sections()) {
    StringRef sectName;
    section.getName(sectName);

    DataRefImpl dataRef = section.getRawDataRefImpl();
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
