#include "ebc/util/BitcodeUtil.h"

#include "catch.hpp"

#include <fstream>

TEST_CASE("Bitcode File Magic Number", "[BitcodeUtil]") {
  const unsigned char valid[4] = {0x42, 0x43, 0xC0, 0xDE};
  REQUIRE(ebc::util::bitcode::IsBitcodeFile((char*)valid));

  const char* invalid = "foobar";
  REQUIRE(!ebc::util::bitcode::IsBitcodeFile(invalid));
}

TEST_CASE("Write Bitcode To File", "[BitcodeUtil]") {
  const char* data = "foobar";
  const char* fileName = "bitcodeutil.test.temp";
  ebc::util::bitcode::WriteBitcodeFile(data, 6, fileName);

  // Compare file content
  std::ifstream in(fileName, std::ios::in | std::ios::binary);
  std::string str((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  REQUIRE(str == data);

  // Cleanup
  REQUIRE(std::remove(fileName) == 0);
}

TEST_CASE("Monotonically Increasing Bitcode File Name", "[BitcodeUtil]") {
  ebc::util::bitcode::FileNamer::SetPrefix("");
  const std::string fileName1 = ebc::util::bitcode::FileNamer::GetFileName();
  const std::string fileName2 = ebc::util::bitcode::FileNamer::GetFileName();

  REQUIRE(stoi(fileName1.substr(0, fileName1.find("."))) < stoi(fileName2.substr(0, fileName2.find("."))));
}

TEST_CASE("Bitcode File Name Prefix", "[BitcodeUtil]") {
  constexpr char prefix[] = "prefix";
  constexpr int length = std::extent<decltype(prefix)>::value;

  ebc::util::bitcode::FileNamer::SetPrefix(prefix);
  const std::string fileName = ebc::util::bitcode::FileNamer::GetFileName();

  REQUIRE(fileName.substr(0, length - 1) == prefix);
}
