#include "ebc/util/BitcodeUtil.h"

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
}
}
}
