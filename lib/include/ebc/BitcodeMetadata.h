#pragma once

#include <memory>
#include <string>
#include <vector>

struct _xmlDoc;
struct _xmlNode;

namespace ebc {
class BitcodeMetadata {
 public:
  BitcodeMetadata(std::string xml);
  BitcodeMetadata(const BitcodeMetadata& bitcodeTOC) = delete;

  ~BitcodeMetadata();

  /// Returns the metadata XML.
  const std::string& GetXml() const;

  /// Get dynamic libraries used for linking.
  std::vector<std::string> GetDylibs() const;

  /// Get options passed to linker.
  std::vector<std::string> GetLinkOptions() const;

  /// Get the commands passed to clang for the given file.
  std::vector<std::string> GetClangCommands(std::string fileName) const;

  /// Get the commands passed to swiftc for the given file.
  std::vector<std::string> GetSwiftCommands(std::string fileName) const;

 private:
  std::vector<std::string> GetCommands(std::string fileName, std::string nodeName) const;

  std::string _xml;
  _xmlDoc* _doc;
  _xmlNode* _root;
};
}
