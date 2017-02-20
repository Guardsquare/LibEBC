#include "ebc/util/Xml.h"

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <iostream>
#include <string>
#include <vector>

namespace ebc {
namespace util {
namespace xml {

std::string GetContent(xmlNode* node) {
  if (node == nullptr) return {};
  auto content = reinterpret_cast<char*>(xmlNodeGetContent(node));
  auto contentStr = std::string(content);
  xmlFree(content);
  return contentStr;
}

xmlNode* FindNodeWithName(xmlNode* root, std::string name) {
  for (auto node = root; node != nullptr; node = node->next) {
    if (node->type == XML_ELEMENT_NODE) {
      if (std::string(reinterpret_cast<const char*>(node->name)) == name) {
        return node;
      }
      auto childNode = FindNodeWithName(node->children, name);
      if (childNode != nullptr) {
        return childNode;
      }
    }
  }
  return nullptr;
}

xmlNode* FindNodeWithNameAndContent(xmlNode* root, std::string name, std::string content) {
  for (auto node = root; node != nullptr; node = node->next) {
    if (node->type == XML_ELEMENT_NODE) {
      if (std::string(reinterpret_cast<const char*>(node->name)) == name && GetContent(node) == content) {
        return node;
      }
      auto childNode = FindNodeWithNameAndContent(node->children, name, content);
      if (childNode != nullptr) {
        return childNode;
      }
    }
  }
  return nullptr;
}

std::vector<std::string> GetTextFromNodesWithName(xmlNode* root, std::string name) {
  std::vector<std::string> contents;
  for (auto node = root; node != nullptr; node = node->next) {
    if (node->type == XML_ELEMENT_NODE && std::string(reinterpret_cast<const char*>(node->name)) == name) {
      contents.push_back(GetContent(node));
    }
  }
  return contents;
}

XmlInitialization::XmlInitialization() {
  ::xmlInitParser();
}

XmlInitialization::~XmlInitialization() {
  ::xmlCleanupParser();
}

XmlInitialization xmlInitialization;
}  // namespace xml
}  // namespace util
}  // namespace ebc
