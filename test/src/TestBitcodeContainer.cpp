#include "ebc/BitcodeContainer.h"
#include "ebc/EmbeddedFile.h"

#include "catch.hpp"

#include <string>
#include <vector>

using namespace ebc;

TEST_CASE("Bitcode Container Commands", "[BitcodeContainer]") {
  auto bitcodeContainer = BitcodeContainer(nullptr, 0);
  std::vector<std::string> commands = {"a", "b", "c"};
  bitcodeContainer.SetCommands(commands);
  REQUIRE(commands == bitcodeContainer.GetCommands());
}

TEST_CASE("Bitcode Container Files", "[BitcodeContainer]") {
  char data[16];
  data[0] = 0x42;
  data[1] = 0x43;
  data[2] = 0xC0;
  data[3] = 0xDE;
  data[4] = 0xFF;
  data[5] = 0xFF;
  data[6] = 0xFF;
  data[7] = 0xFF;
  data[8] = 0x42;
  data[9] = 0x43;
  data[10] = 0xC0;
  data[11] = 0xDE;
  data[12] = 0xFF;
  data[13] = 0xFF;
  data[14] = 0xFF;
  data[15] = 0xFF;

  auto bitcodeContainer = BitcodeContainer(data, 16);
  bitcodeContainer.GetBinaryMetadata().SetFileFormatName("name");

  auto actualFiles = bitcodeContainer.GetEmbeddedFiles();
  REQUIRE(actualFiles.size() == 2);

  for (auto& file : actualFiles) {
    file->Remove();
  }
}

TEST_CASE("Bitcode Container Marker Only", "[BitcodeContainer]") {
  char data[3];
  data[0] = 0x42;
  data[1] = 0x43;
  data[2] = 0xC0;

  auto bitcodeContainer = BitcodeContainer(data, 3);
  bitcodeContainer.GetBinaryMetadata().SetFileFormatName("name");

  auto actualFiles = bitcodeContainer.GetEmbeddedFiles();
  REQUIRE(actualFiles.empty());
}

TEST_CASE("Bitcode Container Prefix", "[BitcodeContainer]") {
  char data[16];
  data[0] = 0x42;
  data[1] = 0x43;
  data[2] = 0xC0;
  data[3] = 0xDE;
  data[4] = 0xFF;
  data[5] = 0xFF;
  data[6] = 0xFF;
  data[7] = 0xFF;
  data[8] = 0x42;
  data[9] = 0x43;
  data[10] = 0xC0;
  data[11] = 0xDE;
  data[12] = 0xFF;
  data[13] = 0xFF;
  data[14] = 0xFF;
  data[15] = 0xFF;

  auto bitcodeContainer = BitcodeContainer(data, 16);
  bitcodeContainer.GetBinaryMetadata().SetFileFormatName("name");

  REQUIRE(bitcodeContainer.GetPrefix() == "");
  bitcodeContainer.SetPrefix("prefix");
  REQUIRE(bitcodeContainer.GetPrefix() == "prefix");

  for (auto& file : bitcodeContainer.GetEmbeddedFiles()) {
    REQUIRE(file->GetName().find("prefix") == 0);
    file->Remove();
  }
}
