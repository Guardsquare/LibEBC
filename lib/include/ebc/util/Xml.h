#pragma once

#include <string>
#include <vector>

struct _xmlDoc;
struct _xmlNode;

namespace ebc {
namespace util {
namespace xml {

/// Get the content of an XML node as string.
std::string GetContent(_xmlNode* node);

/// Finds the first node at the same level or under the given root that has the given name.
_xmlNode* FindNodeWithName(_xmlNode* root, std::string name);

/// Finds the first node at the same level or under the given root that has the given name and content.
_xmlNode* FindNodeWithNameAndContent(_xmlNode* root, std::string name, std::string content);

/// Get the content of each node with the given name at the same level of the given root node.
std::vector<std::string> GetTextFromNodesWithName(_xmlNode* root, std::string name);

class XmlInitialization {
 public:
  XmlInitialization();
  ~XmlInitialization();
};
}
}
}
