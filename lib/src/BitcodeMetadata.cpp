#include "ebc/BitcodeMetadata.h"

#include "ebc/util/Xml.h"

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <utility>

namespace ebc {

BitcodeMetadata::BitcodeMetadata(std::string xml) : _xml(std::move(xml)), _doc(nullptr), _root(nullptr) {
  _doc = xmlReadMemory(_xml.c_str(), _xml.length(), "noname.xml", nullptr, 0);
  _root = xmlDocGetRootElement(_doc);
}

BitcodeMetadata::~BitcodeMetadata() {
  if (_doc != nullptr) {
    xmlFreeDoc(_doc);
    _doc = nullptr;
  }
}

const std::string& BitcodeMetadata::GetXml() const {
  return _xml;
}

std::string BitcodeMetadata::GetVersion() const {
  auto node = util::xml::FindNodeWithName(_root, "version");
  return util::xml::GetContent(node);
}

std::string BitcodeMetadata::GetArchitecture() const {
  auto node = util::xml::FindNodeWithName(_root, "architecture");
  return util::xml::GetContent(node);
}

std::string BitcodeMetadata::GetHideSymbols() const {
  auto node = util::xml::FindNodeWithName(_root, "hide-symbols");
  return util::xml::GetContent(node);
}

std::string BitcodeMetadata::GetPlatform() const {
  auto node = util::xml::FindNodeWithName(_root, "platform");
  return util::xml::GetContent(node);
}

std::string BitcodeMetadata::GetSdkVersion() const {
  auto node = util::xml::FindNodeWithName(_root, "sdkversion");
  return util::xml::GetContent(node);
}

std::vector<std::string> BitcodeMetadata::GetDylibs() const {
  auto node = util::xml::FindNodeWithName(_root, "dylibs");
  if (node != nullptr) {
    return util::xml::GetTextFromNodesWithName(node->children, "lib");
  }
  return std::vector<std::string>();
}

std::vector<std::string> BitcodeMetadata::GetLinkOptions() const {
  auto node = util::xml::FindNodeWithName(_root, "link-options");
  if (node != nullptr) {
    return util::xml::GetTextFromNodesWithName(node->children, "option");
  }
  return std::vector<std::string>();
}

std::vector<std::string> BitcodeMetadata::GetCommands(std::string fileName, std::string nodeName) const {
  auto node = util::xml::FindNodeWithNameAndContent(_root, "name", fileName);
  if (node != nullptr) {
    node = util::xml::FindNodeWithName(node, nodeName);
    if (node != nullptr) {
      return util::xml::GetTextFromNodesWithName(node->children, "cmd");
    }
  }
  return std::vector<std::string>();
}

std::vector<std::string> BitcodeMetadata::GetClangCommands(std::string fileName) const {
  return GetCommands(fileName, "clang");
}

std::vector<std::string> BitcodeMetadata::GetSwiftCommands(std::string fileName) const {
  return GetCommands(fileName, "swift");
}
}  // namespace ebc
