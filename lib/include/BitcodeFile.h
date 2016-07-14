#pragma once

#include <cstdio>
#include <string>
#include <vector>

namespace ebc {
class BitcodeFile {
 public:
  BitcodeFile(std::string name);
  void SetClangCommands(std::vector<std::string>& clangCommands);

  void Remove();

  std::string GetName() const;
  const std::vector<std::string>& GetClangCommands() const;

 private:
  std::string _name;
  std::vector<std::string> _clangCommands;
};
}
