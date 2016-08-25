#include "ebc/util/Namer.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"

namespace ebc {
namespace util {

int Namer::_number = 0;
std::string Namer::_prefix = {};
std::string Namer::_path = {};

void Namer::SetPath(std::string path) {
  _path = path;
}

void Namer::SetPrefix(std::string prefix) {
  _prefix = prefix;
}

std::string Namer::GetFileName() {
  const std::string fileName = _prefix + std::to_string(_number++) + ".bc";
  llvm::SmallString<256> path;
  llvm::sys::path::append(path, _path, fileName);
  return path.str();
}
}  // namespace util
}  // namespace ebc
