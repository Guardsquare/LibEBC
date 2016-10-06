#pragma once

#include <string>
#include <vector>

struct _xmlDoc;
struct _xmlNode;

namespace ebc {
namespace util {
namespace xml {

/// Get the content of an XML node as string.
///
/// @param node The XML node.
///
/// @return The content of the given node.
std::string GetContent(_xmlNode* node);

/// Finds the first node at the same level or under the given root that has the
/// given name.
///
/// @param root The node where the search starts. Nodes at the same level of
///             the root are considered.
/// @param name The name of the desired node.
///
/// @return The first XML node encountered with the given name. Nullptr if none
///         is found.
_xmlNode* FindNodeWithName(_xmlNode* root, std::string name);

/// Finds the first node at the same level or under the given root that has the
/// given name and content.
///
/// @param root The node where the search starts. Nodes on the same level are
///             not considered.
/// @param name The name of the desired node.
/// @param content The content of the desired node.
///
/// @return The first node matching the given criteria. Nullptr if none is
///         found.
_xmlNode* FindNodeWithNameAndContent(_xmlNode* root, std::string name, std::string content);

/// Get the content of each node with the given name at the same level of the
/// given root node.
///
/// @param root The node where the search starts. Nodes on the same level are
///             considered.
/// @param name The name of the desired node.
///
/// @return The content of each node matching the criteria.
std::vector<std::string> GetTextFromNodesWithName(_xmlNode* root, std::string name);

class XmlInitialization {
 public:
  XmlInitialization();
  ~XmlInitialization();
};
}
}
}
