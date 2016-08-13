#include "ebc/util/UUID.h"

#include "catch.hpp"

TEST_CASE("UUID To String", "[BinaryUtil]") {
  std::array<std::uint8_t, 16> uuid = {
      {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'}};
  REQUIRE("30313233-3435-3637-3839-414243444546" == ebc::util::uuid::UuidToString(uuid));
}
