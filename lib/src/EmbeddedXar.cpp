#include "ebc/EmbeddedXar.h"

#include "ebc/BitcodeArchive.h"
#include "ebc/BitcodeMetadata.h"

#include <fstream>
#include <iostream>
#include <utility>

namespace ebc {

std::unique_ptr<BitcodeArchive> EmbeddedXar::GetAsBitcodeArchive() const {
  const std::string file = GetName();

  if (file.empty()) {
    return nullptr;
  }

  std::ifstream input(file, std::ifstream::binary);

  if (!input) {
    std::cerr << "Unable to open " << file << std::endl;
    return nullptr;
  }

  std::vector<char> buffer((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

  return std::make_unique<BitcodeArchive>(&buffer[0], buffer.size());
}
}  // namespace ebc
