#include "BitcodeArchive.h"

#include "catch.hpp"

TEST_CASE("Bitcode Archive Name", "[BitcodeArchive]") {
  const auto bitcodeArchive = ebc::BitcodeArchive("Name", {}, nullptr, 0);
  REQUIRE("Name" == bitcodeArchive.GetName());
}

TEST_CASE("Bitcode Archive UUID", "[BitcodeArchive]") {
  std::uint8_t uuid[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  const auto bitcodeArchive = ebc::BitcodeArchive("Name", uuid, nullptr, 0);
  REQUIRE("30313233-3435-3637-3839-414243444546" == bitcodeArchive.GetUUID());
}
