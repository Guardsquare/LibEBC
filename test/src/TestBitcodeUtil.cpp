#include "ebc/util/Bitcode.h"

#include "catch.hpp"

#include <fstream>

TEST_CASE("Bitcode File Magic Number", "[BitcodeUtil]") {
  const unsigned char valid[4] = {0x0B, 0x17, 0xC0, 0xDE};
  REQUIRE(ebc::util::bitcode::IsBitcode((char*)valid));

  const char* invalid = "foobar";
  REQUIRE(!ebc::util::bitcode::IsBitcode(invalid));
}

TEST_CASE("Write Bitcode To File", "[BitcodeUtil]") {
  const char* data = "foobar";
  const char* fileName = "bitcodeutil.test.temp";
  ebc::util::bitcode::WriteToFile(data, 6, fileName);

  // Compare file content
  std::ifstream in(fileName, std::ios::in | std::ios::binary);
  std::string str((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  REQUIRE(str == data);

  // Cleanup
  REQUIRE(std::remove(fileName) == 0);
}
