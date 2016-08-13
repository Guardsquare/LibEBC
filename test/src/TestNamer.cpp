#include "ebc/util/Namer.h"

#include "catch.hpp"

TEST_CASE("Monotonically Increasing Bitcode File Name", "[Namer]") {
  ebc::util::Namer::SetPrefix("");
  const std::string fileName1 = ebc::util::Namer::GetFileName();
  const std::string fileName2 = ebc::util::Namer::GetFileName();

  REQUIRE(stoi(fileName1.substr(0, fileName1.find("."))) < stoi(fileName2.substr(0, fileName2.find("."))));
}

TEST_CASE("Bitcode File Name Prefix", "[Namer]") {
  constexpr char prefix[] = "prefix";
  constexpr int length = std::extent<decltype(prefix)>::value;

  ebc::util::Namer::SetPrefix(prefix);
  const std::string fileName = ebc::util::Namer::GetFileName();

  REQUIRE(fileName.substr(0, length - 1) == prefix);
}
