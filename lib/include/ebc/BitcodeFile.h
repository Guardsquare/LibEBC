#pragma once

#include <string>
#include <vector>

namespace ebc {
class BitcodeFile {
 public:
  BitcodeFile(std::string name);

  std::string GetName() const;

  /// Get all commands passed to the compiler to create this bitcode file.
  ///
  /// @return A vector of strings with the clang front-end commands.
  const std::vector<std::string>& GetCommands() const;

  /// Set all commands passed to the compiler to create this bitcode file.
  ///
  /// @param clangCommands A vector of strings with the clang front-end
  ///                      commands.
  void SetCommands(const std::vector<std::string>& clangCommands);

  /// Remove the actual bitcode file from the file system.
  void Remove();

 private:
  std::string _name;
  std::vector<std::string> _commands;
};

inline bool operator==(const BitcodeFile& lhs, const BitcodeFile& rhs) {
  return lhs.GetName() == rhs.GetName();
}
}
