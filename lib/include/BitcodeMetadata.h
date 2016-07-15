#pragma once

extern "C" {
#include <libxml/parser.h>
#include <libxml/tree.h>
}

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ebc {
class BitcodeMetadata {
 public:
  BitcodeMetadata(std::string xml);
  BitcodeMetadata(const BitcodeMetadata& bitcodeTOC) = delete;

  ~BitcodeMetadata();

  const std::string& GetXml() const;

  std::vector<std::string> GetDylibs() const;
  std::vector<std::string> GetLinkOptions() const;

  std::vector<std::string> GetClangCommands(std::string fileName) const;
  std::vector<std::string> GetSwiftCommands(std::string fileName) const;

 private:
  std::vector<std::string> GetCommands(std::string fileName, std::string nodeName) const;

  std::string _xml;
  xmlDoc* _doc;
  xmlNode* _root;
};
}
