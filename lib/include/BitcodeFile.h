#pragma once

#include <cstdio>
#include <string>
#include <vector>

namespace ebc {
class BitcodeFile {
 public:
  BitcodeFile(std::string name);

  std::string GetName() const;

  /// Get all compiler commands passed to clang for create this bitcode file.
  const std::vector<std::string>& GetClangCommands() const;
  void SetClangCommands(std::vector<std::string>& clangCommands);

  /// Get all compiler commands passed to swiftc for create this bitcode file.
  const std::vector<std::string>& GetSwiftCommands() const;
  void SetSwiftCommands(std::vector<std::string>& swiftCommands);

  /// Remove the actual bitcode file from the filesystem.
  void Remove();

 private:
  std::string _name;
  std::vector<std::string> _clangCommands;
  std::vector<std::string> _swiftCommands;
};
}
