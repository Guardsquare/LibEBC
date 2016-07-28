#include "ebc/BitcodeContainer.h"
#include "ebc/BitcodeFile.h"

#include "catch.hpp"

#include <string>
#include <vector>

TEST_CASE("Bitcode Container Name", "[BitcodeContainer]") {
  auto bitcodeContainer = ebc::BitcodeContainer(nullptr, 0);
  bitcodeContainer.SetName("name");
  REQUIRE("name" == bitcodeContainer.GetName());
}

TEST_CASE("Bitcode Container UUID", "[BitcodeContainer]") {
  auto bitcodeContainer = ebc::BitcodeContainer(nullptr, 0);
  std::uint8_t uuid[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  bitcodeContainer.SetUuid(uuid);
  REQUIRE("30313233-3435-3637-3839-414243444546" == bitcodeContainer.GetUUID());
}

TEST_CASE("Bitcode Container Commands", "[BitcodeContainer]") {
  auto bitcodeContainer = ebc::BitcodeContainer(nullptr, 0);
  std::vector<std::string> commands = {"a", "b", "c"};
  bitcodeContainer.SetCommands(commands);
  REQUIRE(commands == bitcodeContainer.GetCommands());
}

TEST_CASE("Bitcode Container Files", "[BitcodeContainer]") {
  using ebc::BitcodeFile;
  const char data[] = "BC000BC111BC222";
  auto bitcodeContainer = ebc::BitcodeContainer(data, 15);
  bitcodeContainer.SetName("name");
  const std::vector<BitcodeFile> expectedFiles = {BitcodeFile("name_0.bc"), BitcodeFile("name_1.bc"),
                                                  BitcodeFile("name_2.bc")};

  auto actualFiles = bitcodeContainer.GetBitcodeFiles();
  REQUIRE(expectedFiles == actualFiles);

  for (auto& file : actualFiles) {
    file.Remove();
  }
}
