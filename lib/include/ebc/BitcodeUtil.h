#pragma once

#include <array>
#include <fstream>
#include <string>

namespace ebc {
namespace util {

inline bool IsBitcodeFile(const char *data) {
  return static_cast<unsigned char>(data[0]) == 0x42 && static_cast<unsigned char>(data[1]) == 0x43 &&
         static_cast<unsigned char>(data[2]) == 0xC0 && static_cast<unsigned char>(data[3]) == 0xDE;
}

inline void WriteBitcodeFile(const char *data, std::uint32_t size, std::string name) {
  std::ofstream outfile(name, std::ofstream::binary);
  outfile.write(data, size);
  outfile.close();
}

inline std::string MakeBitcodeFileName(std::string prefix, std::string name, int i) {
  return prefix.append(name).append(".").append(std::to_string(i)).append(".bc");
}

inline std::string UuidToString(std::array<std::uint8_t, 16> uuid) {
  char buffer[37];
  sprintf(buffer, "%2.2X%2.2X%2.2X%2.2X-%2.2X%2.2X-%2.2X%2.2X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X", uuid[0],
          uuid[1], uuid[2], uuid[3], uuid[4], uuid[5], uuid[6], uuid[7], uuid[8], uuid[9], uuid[10], uuid[11], uuid[12],
          uuid[13], uuid[14], uuid[15]);
  return std::string(buffer);
}
}
}
