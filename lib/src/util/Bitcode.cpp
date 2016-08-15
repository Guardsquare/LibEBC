#include "ebc/util/Bitcode.h"

#include "ebc/Config.h"

#include <fstream>
#include <iostream>

namespace ebc {
namespace util {
namespace bitcode {

bool IsBitcodeFile(const char *data) {
  return static_cast<unsigned char>(data[0]) == 0x42 && static_cast<unsigned char>(data[1]) == 0x43 &&
         static_cast<unsigned char>(data[2]) == 0xC0 && static_cast<unsigned char>(data[3]) == 0xDE;
}

void WriteFile(const char *data, std::uint32_t size, std::string fileName) {
  std::ofstream outfile(fileName, std::ofstream::binary);

  if (!outfile) {
    std::cerr << "Unable to open " << fileName << std::endl;
    return;
  }

  outfile.write(data, size);
  outfile.close();
}

bool HasXar() {
#ifdef HAVE_LIBXAR
  return true;
#else
  return false;
#endif
}

}  // namespace bitcode
}  // namespace util
}  // namespace ebc
