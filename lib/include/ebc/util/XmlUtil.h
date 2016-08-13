#pragma once

#include <string>
#include <vector>

struct _xmlDoc;
struct _xmlNode;

namespace ebc {
namespace util {
namespace xml {

std::string GetContent(_xmlNode* node);

_xmlNode* FindNodeWithName(_xmlNode* root, std::string name);

_xmlNode* FindNodeWithNameAndContent(_xmlNode* root, std::string name, std::string content);

std::vector<std::string> GetTextFromNodesWithName(_xmlNode* root, std::string name);

class XmlInitialization {
 public:
  XmlInitialization();
  ~XmlInitialization();
};
}
}
}
