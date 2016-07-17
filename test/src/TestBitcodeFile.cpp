#include "BitcodeFile.h"

#include "catch.hpp"

TEST_CASE("Bitcode File Name", "[BitcodeFile]") {
  const auto bitcodeFile = ebc::BitcodeFile("Name");
  REQUIRE("Name" == bitcodeFile.GetName());
}

TEST_CASE("Bitcode File No Clang Commands", "[BitcodeFile]") {
  auto bitcodeFile = ebc::BitcodeFile("Name");
  REQUIRE(bitcodeFile.GetClangCommands().empty());
}

TEST_CASE("Bitcode File Clang Commands", "[BitcodeFile]") {
  auto bitcodeFile = ebc::BitcodeFile("Name");
  std::vector<std::string> commands = {"a", "b", "c"};
  bitcodeFile.SetClangCommands(commands);
  REQUIRE(commands == bitcodeFile.GetClangCommands());
}

TEST_CASE("Bitcode File No Swift Commands", "[BitcodeFile]") {
  auto bitcodeFile = ebc::BitcodeFile("Name");
  REQUIRE(bitcodeFile.GetSwiftCommands().empty());
}

TEST_CASE("Bitcode File Swift Commands", "[BitcodeFile]") {
  auto bitcodeFile = ebc::BitcodeFile("Name");
  std::vector<std::string> commands = {"a", "b", "c"};
  bitcodeFile.SetClangCommands(commands);
  REQUIRE(commands == bitcodeFile.GetClangCommands());
}
