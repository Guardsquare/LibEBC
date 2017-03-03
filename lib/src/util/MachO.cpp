#include "ebc/util/MachO.h"

#include <fstream>
#include <iostream>

namespace ebc {
namespace util {
namespace macho {

bool IsMachO(std::uint32_t magic) {
  switch (magic) {
    case FAT_MAGIC:
    case FAT_CIGAM:
    case MH_MAGIC:
    case MH_CIGAM:
    case MH_MAGIC_64:
    case MH_CIGAM_64:
      return true;
    default:
      return false;
  }
  return false;
}

bool IsMachOFile(std::string file) {
  // Read first four bytes and check magic number.
  std::ifstream input(file, std::ifstream::binary);

  if (!input) {
    std::cerr << "Unable to open " << file << std::endl;
    return false;
  }

  char buffer[4];
  input.read(buffer, 4);
  return IsMachO(*reinterpret_cast<std::uint32_t*>(buffer));
}
}  // namespace macho
}  // namespace util
}  // namespace ebc
