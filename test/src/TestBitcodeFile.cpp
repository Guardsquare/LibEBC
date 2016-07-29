#include "ebc/BitcodeFile.h"

#include "catch.hpp"

TEST_CASE("Bitcode File Name", "[BitcodeFile]") {
  const auto bitcodeFile = ebc::BitcodeFile("Name");
  REQUIRE("Name" == bitcodeFile.GetName());
}

TEST_CASE("Bitcode File No  Commands", "[BitcodeFile]") {
  auto bitcodeFile = ebc::BitcodeFile("Name");
  REQUIRE(bitcodeFile.GetCommands().empty());
}

TEST_CASE("Bitcode File Commands", "[BitcodeFile]") {
  auto bitcodeFile = ebc::BitcodeFile("Name");
  std::vector<std::string> commands = {"a", "b", "c"};
  bitcodeFile.SetCommands(commands);
  REQUIRE(commands == bitcodeFile.GetCommands());
}
