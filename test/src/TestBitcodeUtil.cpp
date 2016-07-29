#include "ebc/BitcodeUtil.h"

#include "catch.hpp"

#include <cstdio>

TEST_CASE("Bitcode File Magic Number", "[BitcodeUtil]") {
  const unsigned char valid[4] = {0x42, 0x43, 0xC0, 0xDE};
  REQUIRE(ebc::util::IsBitcodeFile((char*)valid));

  const char* invalid = "foobar";
  REQUIRE(!ebc::util::IsBitcodeFile(invalid));
}

TEST_CASE("Write Bitcode To File", "[BitcodeUtil]") {
  const char* data = "foobar";
  const char* fileName = "bitcodeutil.test.temp";
  ebc::util::WriteBitcodeFile(data, 6, fileName);

  // Compare file content
  std::ifstream in(fileName, std::ios::in | std::ios::binary);
  std::string str((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  REQUIRE(str == data);

  // Cleanup
  REQUIRE(std::remove(fileName) == 0);
}
TEST_CASE("UUID To String", "[BitcodeUtil]") {
  std::array<std::uint8_t, 16> uuid = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  REQUIRE("30313233-3435-3637-3839-414243444546" == ebc::util::UuidToString(uuid));
}
