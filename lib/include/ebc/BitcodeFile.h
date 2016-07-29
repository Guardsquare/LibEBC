#pragma once

#include <string>
#include <vector>

namespace ebc {
class BitcodeFile {
 public:
  BitcodeFile(std::string name);

  std::string GetName() const;

  /// Get all commands passed to the compiler to create this bitcode file.
  const std::vector<std::string>& GetCommands() const;
  void SetCommands(const std::vector<std::string>& clangCommands);

  /// Remove the actual bitcode file from the filesystem.
  void Remove();

 private:
  std::string _name;
  std::vector<std::string> _commands;
};

inline bool operator==(const BitcodeFile& lhs, const BitcodeFile& rhs) {
  return lhs.GetName() == rhs.GetName();
}
}
