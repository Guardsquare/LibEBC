#include "ebc/EmbeddedFileFactory.h"

#include "ebc/EmbeddedBitcode.h"
#include "ebc/EmbeddedExports.h"
#include "ebc/EmbeddedFile.h"
#include "ebc/EmbeddedObject.h"
#include "ebc/EmbeddedXar.h"

#include "ebc/util/Bitcode.h"
#include "ebc/util/MachO.h"
#include "ebc/util/Xar.h"

#include <cassert>

namespace ebc {

static EmbeddedFile::Type GetTypeFromString(std::string fileType) {
  if (fileType == "Bitcode") {
    return EmbeddedFile::Type::Bitcode;
  } else if (fileType == "Exports") {
    return EmbeddedFile::Type::Exports;
  } else if (fileType == "Bundle") {
    return EmbeddedFile::Type::Xar;
  } else if (fileType == "LTO") {
    return EmbeddedFile::Type::LTO;
  } else if (fileType == "Object") {
    return EmbeddedFile::Type::Object;
  }
  return EmbeddedFile::Type::File;
}

std::unique_ptr<EmbeddedFile> EmbeddedFileFactory::CreateEmbeddedFile(std::string file) {
  BitcodeType bitcodeType = util::bitcode::GetBitcodeType(file);
  if (bitcodeType != BitcodeType::Unknown) {
    return std::unique_ptr<EmbeddedFile>(new EmbeddedBitcode(std::move(file), bitcodeType));
  }

  if (util::xar::IsXarFile(file)) {
    return std::unique_ptr<EmbeddedFile>(new EmbeddedXar(std::move(file)));
  }

  if (util::macho::IsMachOFile(file)) {
    return std::unique_ptr<EmbeddedFile>(new EmbeddedObject(std::move(file)));
  }

  return std::make_unique<EmbeddedFile>(file);
}

std::unique_ptr<EmbeddedFile> EmbeddedFileFactory::CreateEmbeddedFile(std::string file, std::string fileType) {
  if (fileType.empty()) {
    return CreateEmbeddedFile(std::move(file));
  }

  const auto type = GetTypeFromString(std::move(fileType));
  switch (type) {
    case EmbeddedFile::Type::LTO:
      return std::unique_ptr<EmbeddedFile>(new EmbeddedBitcode(std::move(file), BitcodeType::LTO));
    case EmbeddedFile::Type::Bitcode: {
      BitcodeType bitcodeType = util::bitcode::GetBitcodeType(file);
      if (bitcodeType != BitcodeType::Unknown) {
        return std::unique_ptr<EmbeddedFile>(new EmbeddedBitcode(std::move(file), bitcodeType));
      } else {
        assert(false && "Filetype says bitcode but file itself is not recognized as such");
      }
      break;
    }
    case EmbeddedFile::Type::Exports:
      return std::unique_ptr<EmbeddedFile>(new EmbeddedExports(std::move(file)));
    case EmbeddedFile::Type::File:
      return std::make_unique<EmbeddedFile>(file);
    case EmbeddedFile::Type::Object:
      return std::unique_ptr<EmbeddedFile>(new EmbeddedObject(std::move(file)));
    case EmbeddedFile::Type::Xar:
      return std::unique_ptr<EmbeddedFile>(new EmbeddedXar(std::move(file)));
  }

  return std::make_unique<EmbeddedFile>(file);
}
}  // namespace ebc
