#include "XmlHelper.h"
#include "gtest/gtest.h"

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <string>
#include <vector>

class XmlHelperTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    _doc = xmlNewDoc(BAD_CAST "1.0");
    _root = xmlNewNode(nullptr, BAD_CAST "root");
    _node1 = xmlNewChild(_root, nullptr, BAD_CAST "node1", BAD_CAST "content1");

    _node2 = xmlNewNode(NULL, BAD_CAST "node2");
    xmlAddChild(_root, _node2);
    _child1 = xmlNewChild(_node2, nullptr, BAD_CAST "child", BAD_CAST "child1");
    _child2 = xmlNewChild(_node2, nullptr, BAD_CAST "child", BAD_CAST "child2");
    _child3 = xmlNewChild(_node2, nullptr, BAD_CAST "child", BAD_CAST "child3");
  }

  virtual void TearDown() {
    xmlFreeDoc(_doc);
  }

  xmlDoc* _doc;
  xmlNode* _root;
  xmlNode* _node1;
  xmlNode* _node2;
  xmlNode* _child1;
  xmlNode* _child2;
  xmlNode* _child3;
};

TEST_F(XmlHelperTest, GetContent) {
  auto content = ebc::xml::GetContent(_node1);
  ASSERT_EQ("content1", content);
}

TEST_F(XmlHelperTest, FindNodeWithName) {
  auto node1 = ebc::xml::FindNodeWithName(_root, "node1");
  ASSERT_EQ(_node1, node1);

  auto node2 = ebc::xml::FindNodeWithName(_root, "node2");
  ASSERT_EQ(_node2, node2);
}

TEST_F(XmlHelperTest, FindNodeWithNameAndContent) {
  auto child1 = ebc::xml::FindNodeWithNameAndContent(_root, "child", "child1");
  ASSERT_EQ(_child1, child1);

  auto child2 = ebc::xml::FindNodeWithNameAndContent(_root, "child", "child2");
  ASSERT_EQ(_child2, child2);

  auto child3 = ebc::xml::FindNodeWithNameAndContent(_root, "child", "child3");
  ASSERT_EQ(_child3, child3);
}

TEST_F(XmlHelperTest, GetTextFromNodesWithName) {
  auto nodeList = ebc::xml::GetTextFromNodesWithName(_node2->children, "child");
  std::vector<std::string> expected = {"child1", "child2", "child3"};
  ASSERT_EQ(expected, nodeList);
}
