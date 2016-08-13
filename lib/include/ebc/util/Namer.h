#pragma once

#include <string>

namespace ebc {
namespace util {

class Namer {
 public:
  Namer() = delete;
  static std::string GetFileName();
  static void SetPrefix(std::string prefix);

 private:
  static std::string _prefix;
  static int _number;
};
}
}
