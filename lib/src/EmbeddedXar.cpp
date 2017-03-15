#include "ebc/EmbeddedXar.h"

#include "ebc/BitcodeArchive.h"
#include "ebc/BitcodeContainer.h"

namespace ebc {

std::unique_ptr<BitcodeContainer> EmbeddedXar::GetAsBitcodeArchive() const {
  auto bitcodeArchive = BitcodeArchive::BitcodeArchiveFromFile(GetName());
  bitcodeArchive->GetBinaryMetadata().SetFileName(GetName());
  return bitcodeArchive;
}
}  // namespace ebc
