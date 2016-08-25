#pragma once

#include <string>

namespace ebc {
namespace util {

/// Static class for generating unique file names with a monotonically increasing number.
class Namer {
 public:
  Namer() = delete;

  /// Get a new unique file name. The file name number increases by one after this call.
  static std::string GetFileName();

  /// Set a destination directory path.
  static void SetPath(std::string path);

  /// Set a global file name prefix. All subsequent file names will start with this prefix.
  static void SetPrefix(std::string prefix);

 private:
  static std::string _path;
  static std::string _prefix;
  static int _number;
};
}
}
