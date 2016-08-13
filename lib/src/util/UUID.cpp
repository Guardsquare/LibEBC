#include "ebc/util/UUID.h"

namespace ebc {
namespace util {
namespace uuid {

std::string UuidToString(std::array<std::uint8_t, 16> uuid) {
  char buffer[37];
  std::sprintf(buffer, "%2.2X%2.2X%2.2X%2.2X-%2.2X%2.2X-%2.2X%2.2X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X", uuid[0],
               uuid[1], uuid[2], uuid[3], uuid[4], uuid[5], uuid[6], uuid[7], uuid[8], uuid[9], uuid[10], uuid[11],
               uuid[12], uuid[13], uuid[14], uuid[15]);
  return std::string(buffer);
}
}
}  // namespace util
}  // namespace ebc
