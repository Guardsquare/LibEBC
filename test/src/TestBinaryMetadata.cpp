#include "ebc/BinaryMetadata.h"

#include "catch.hpp"

#include <string>
#include <vector>

TEST_CASE("File Format Name", "[BinaryMetadata]") {
  BinaryMetadata binaryMetadata;
  binaryMetadata.SetFileFormatName("name");
  REQUIRE("name" == BinaryMetadata.GetFileFormatName());
}

TEST_CASE("File Name", "[BinaryMetadata]") {
  BinaryMetadata binaryMetadata;
  binaryMetadata.SetFileName("name");
  REQUIRE("name" == BinaryMetadata.GetFileName());
}

TEST_CASE("UUID", "[BinaryMetadata]") {
  constexpr std::uint8_t uuid[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  BinaryMetadata binaryMetadata;
  binaryMetadata.SetUuid(uuid);
  REQUIRE("30313233-3435-3637-3839-414243444546" == binaryMetadata.GetUUID());
}
