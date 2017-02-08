#pragma once

#include <string>
#include <vector>

namespace ebc {
class EmbeddedFile {
 public:
  EmbeddedFile(std::string name);
  virtual ~EmbeddedFile() = default;

  std::string GetName() const;

  /// Get all commands passed to the compiler to create this embedded file.
  ///
  /// @return A vector of strings with the clang front-end commands.
  const std::vector<std::string>& GetCommands() const;

  /// Set all commands passed to the compiler to create this embedded file.
  ///
  /// @param commands A vector of strings with the clang front-end
  ///                 commands.
  void SetCommands(const std::vector<std::string>& commands);

  /// Remove the underlying file from the file system.
  void Remove() const;

  enum class Type {
    Bitcode,
    MachO,
    Unknown,
    Xar,
  };

  Type GetType() const;

 protected:
  EmbeddedFile(std::string name, Type type);

 private:
  std::string _name;
  Type _type;
  std::vector<std::string> _commands;
};

inline bool operator==(const EmbeddedFile& lhs, const EmbeddedFile& rhs) {
  return lhs.GetName() == rhs.GetName();
}
}
