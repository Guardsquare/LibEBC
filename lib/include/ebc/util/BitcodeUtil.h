#pragma once

#include <cstdint>
#include <string>

namespace ebc {
namespace util {
namespace bitcode {

class FileNamer {
 public:
  FileNamer() = delete;
  static std::string GetFileName();
  static void SetPrefix(std::string prefix);

 private:
  static std::string _prefix;
  static int _number;
};

bool IsBitcodeFile(const char *data);
void WriteBitcodeFile(const char *data, std::uint32_t size, std::string name);
bool HasXar();
}
}
}
