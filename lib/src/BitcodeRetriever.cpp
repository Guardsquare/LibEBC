#include "BitcodeRetriever.h"

#include "llvm/Object/MachO.h"
#include "llvm/Object/MachOUniversal.h"

#include <memory>
#include <string>

using namespace llvm;

namespace ebc {

BitcodeRetriever::BitcodeRetriever(std::string objectPath) : _objectPath(objectPath) {}

std::vector<BitcodeArchive> BitcodeRetriever::GetBitcodeArchives() {
  auto bitcodeArchives = std::vector<BitcodeArchive>();
  auto memoryBuffer = MemoryBuffer::getFile(_objectPath);
  if (!memoryBuffer) return bitcodeArchives;

  std::error_code error;
  auto mMachOUniversalBinary =
      std::make_unique<object::MachOUniversalBinary>((*memoryBuffer)->getMemBufferRef(), error);

  if (error) return bitcodeArchives;

  for (auto object : mMachOUniversalBinary->objects()) {
    if (auto machOObject = object.getAsObjectFile()) {
      auto machOObj = std::move(*machOObject);

      std::string name = machOObj->getFileFormatName().str();

      for (const object::SectionRef &section : machOObj->sections()) {
        StringRef sectName;
        section.getName(sectName);

        object::DataRefImpl dataRef = section.getRawDataRefImpl();
        StringRef segName = machOObj->getSectionFinalSegmentName(dataRef);

        if (segName == "__LLVM" && sectName == "__bundle") {
          StringRef bytesStr;
          section.getContents(bytesStr);
          const char *sect = reinterpret_cast<const char *>(bytesStr.data());
          uint32_t sect_size = bytesStr.size();

          auto uuid = machOObj->getUuid();
          bitcodeArchives.push_back(BitcodeArchive(name, uuid.data(), sect, sect_size));
        }
      }
    }
  }
  return bitcodeArchives;
}
}
