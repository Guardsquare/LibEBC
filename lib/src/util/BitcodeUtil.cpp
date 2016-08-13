#include "ebc/util/BitcodeUtil.h"

#include "ebc/Config.h"

#include <fstream>

namespace ebc {
namespace util {
namespace bitcode {

int FileNamer::_number = 0;
std::string FileNamer::_prefix = {};

void FileNamer::SetPrefix(std::string prefix) {
  _prefix = prefix;
}

std::string FileNamer::GetFileName() {
  return _prefix + std::to_string(_number++) + ".bc";
}

bool IsBitcodeFile(const char *data) {
  return static_cast<unsigned char>(data[0]) == 0x42 && static_cast<unsigned char>(data[1]) == 0x43 &&
         static_cast<unsigned char>(data[2]) == 0xC0 && static_cast<unsigned char>(data[3]) == 0xDE;
}

void WriteBitcodeFile(const char *data, std::uint32_t size, std::string name) {
  std::ofstream outfile(name, std::ofstream::binary);
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
