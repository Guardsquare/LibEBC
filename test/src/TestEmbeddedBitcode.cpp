#include "ebc/EmbeddedBitcode.h"

#include "catch.hpp"

TEST_CASE("Bitcode File Name", "[EmbeddedBitcode]") {
  const auto bitcodeFile = ebc::EmbeddedBitcode("Name");
  REQUIRE("Name" == bitcodeFile.GetName());
}

TEST_CASE("Bitcode File No  Commands", "[EmbeddedBitcode]") {
  auto bitcodeFile = ebc::EmbeddedBitcode("Name");
  REQUIRE(bitcodeFile.GetCommands().empty());
}

TEST_CASE("Bitcode File Commands", "[EmbeddedBitcode]") {
  auto bitcodeFile = ebc::EmbeddedBitcode("Name");
  std::vector<std::string> commands = {"a", "b", "c"};
  bitcodeFile.SetCommands(commands);
  REQUIRE(commands == bitcodeFile.GetCommands());
}
