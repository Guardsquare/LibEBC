#include "ebc/EmbeddedBitcode.h"

#include "catch.hpp"

using namespace ebc;

TEST_CASE("Bitcode File Name", "[EmbeddedBitcode]") {
  const auto bitcodeFile = EmbeddedBitcode("Name");
  REQUIRE("Name" == bitcodeFile.GetName());
}

TEST_CASE("Bitcode File No  Commands", "[EmbeddedBitcode]") {
  auto bitcodeFile = EmbeddedBitcode("Name");
  REQUIRE(bitcodeFile.GetCommands().empty());
}

TEST_CASE("Bitcode File Commands", "[EmbeddedBitcode]") {
  auto bitcodeFile = EmbeddedBitcode("Name");
  std::vector<std::string> commands = {"a", "b", "c"};
  bitcodeFile.SetCommands(commands, EmbeddedFile::CommandSource::Clang);
  REQUIRE(commands == bitcodeFile.GetCommands());
  REQUIRE(EmbeddedFile::CommandSource::Clang == bitcodeFile.GetCommandSource());
}
