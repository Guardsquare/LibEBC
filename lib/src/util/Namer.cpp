#include "ebc/util/Namer.h"

namespace ebc {
namespace util {

int Namer::_number = 0;
std::string Namer::_prefix = {};

void Namer::SetPrefix(std::string prefix) {
  _prefix = prefix;
}

std::string Namer::GetFileName() {
  return _prefix + std::to_string(_number++) + ".bc";
}
}  // namespace util
}  // namespace ebc
