#include "ebc/BinaryMetadata.h"

#include "catch.hpp"

#include <string>
#include <vector>

TEST_CASE("File Format Name", "[BinaryMetadata]") {
  ebc::BinaryMetadata binaryMetadata;
  binaryMetadata.SetFileFormatName("name");
  REQUIRE("name" == binaryMetadata.GetFileFormatName());
}

TEST_CASE("File Name", "[BinaryMetadata]") {
  ebc::BinaryMetadata binaryMetadata;
  binaryMetadata.SetFileName("name");
  REQUIRE("name" == binaryMetadata.GetFileName());
}

TEST_CASE("Architecture", "[BinaryMetadata]") {
  ebc::BinaryMetadata binaryMetadata;
  binaryMetadata.SetArch("arm64");
  REQUIRE("arm64" == binaryMetadata.GetArch());
}

TEST_CASE("UUID", "[BinaryMetadata]") {
  constexpr std::uint8_t uuid[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  ebc::BinaryMetadata binaryMetadata;
  binaryMetadata.SetUuid(uuid);
  REQUIRE("30313233-3435-3637-3839-414243444546" == binaryMetadata.GetUUID());
}
