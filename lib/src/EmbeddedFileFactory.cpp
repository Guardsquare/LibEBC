#include "ebc/EmbeddedFileFactory.h"

#include "ebc/EmbeddedBitcode.h"
#include "ebc/EmbeddedFile.h"
#include "ebc/EmbeddedMachO.h"
#include "ebc/EmbeddedXar.h"

#include "ebc/util/Bitcode.h"
#include "ebc/util/MachO.h"
#include "ebc/util/Xar.h"

namespace ebc {

std::unique_ptr<EmbeddedFile> EmbeddedFileFactory::CreateEmbeddedFile(std::string file) {
  if (util::bitcode::IsBitcodeFile(file)) {
    return std::unique_ptr<EmbeddedFile>(new EmbeddedBitcode(std::move(file)));
  }

  if (util::xar::IsXarFile(file)) {
    return std::unique_ptr<EmbeddedFile>(new EmbeddedXar(std::move(file)));
  }

  if (util::macho::IsMachOFile(file)) {
    return std::unique_ptr<EmbeddedFile>(new EmbeddedMachO(std::move(file)));
  }

  return nullptr;
}
}
