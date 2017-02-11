#include "ebc/BitcodeType.h"
#include "ebc/util/Bitcode.h"

#include "catch.hpp"

#include <fstream>

using namespace ebc;

TEST_CASE("Bitcode File Magic Number", "[BitcodeUtil]") {
  std::uint64_t wrapper = 0x000000000B17C0DE;
  std::uint64_t bitcode = 0xFFFFFFFF0B17C0DE;
  std::uint64_t unknown = 0xFFFFFFFFFFFFFFFF;
  std::uint64_t ir = 0xFFFFFFFF4243C0DE;

  REQUIRE(util::bitcode::GetBitcodeType(wrapper) == BitcodeType::BitcodeWrapper);
  REQUIRE(util::bitcode::GetBitcodeType(bitcode) == BitcodeType::Bitcode);
  REQUIRE(util::bitcode::GetBitcodeType(unknown) == BitcodeType::Unknown);
  REQUIRE(util::bitcode::GetBitcodeType(ir) == BitcodeType::IR);
}

TEST_CASE("Write Bitcode To File", "[BitcodeUtil]") {
  const char* data = "foobar";
  const char* fileName = "bitcodeutil.test.temp";
  util::bitcode::WriteToFile(data, 6, fileName);

  // Compare file content
  std::ifstream in(fileName, std::ios::in | std::ios::binary);
  std::string str((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  REQUIRE(str == data);

  // Cleanup
  REQUIRE(std::remove(fileName) == 0);
}
