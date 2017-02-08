#include "ebc/util/Bitcode.h"

#include "ebc/Config.h"

#include <fstream>
#include <iostream>

namespace ebc {
namespace util {
namespace bitcode {

bool IsBitcode(std::uint32_t magic) {
  switch (magic) {
    case BC_MAGIC:
    case BC_CIGAM:
    case IR_MAGIC:
    case IR_CIGAM:
      return true;
    default:
      return false;
  }
  return false;
}

bool IsBitcode(const char *const data) {
  return IsBitcode(*reinterpret_cast<const std::uint32_t *>(data));
}

bool IsBitcodeFile(std::string file) {
  // Read first four bytes and check magic number.
  std::ifstream input(file, std::ifstream::binary);

  if (!input) {
    std::cerr << "Unable to open " << file << std::endl;
    return false;
  }

  char buffer[4];
  input.read(buffer, 4);
  return IsBitcode(buffer);
}

void WriteToFile(const char *data, std::uint32_t size, std::string file) {
  std::ofstream output(file, std::ofstream::binary);

  if (!output) {
    std::cerr << "Unable to open " << file << std::endl;
    return;
  }

  output.write(data, size);
  output.close();
}

}  // namespace bitcode
}  // namespace util
}  // namespace ebc
