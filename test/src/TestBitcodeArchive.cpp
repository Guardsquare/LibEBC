#include "BitcodeArchive.h"

#include "catch.hpp"

TEST_CASE("Bitcode Archive Name", "[BitcodeArchive]") {
  auto bitcodeArchive = ebc::BitcodeArchive(nullptr, 0);
  bitcodeArchive.SetName("Name");
  REQUIRE("Name" == bitcodeArchive.GetName());
}

TEST_CASE("Bitcode Archive UUID", "[BitcodeArchive]") {
  auto bitcodeArchive = ebc::BitcodeArchive(nullptr, 0);
  std::uint8_t uuid[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  bitcodeArchive.SetUuid(uuid);
  REQUIRE("30313233-3435-3637-3839-414243444546" == bitcodeArchive.GetUUID());
}
