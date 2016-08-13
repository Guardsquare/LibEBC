#pragma once

#include <cstdint>
#include <string>

namespace ebc {
namespace util {
namespace bitcode {

bool IsBitcodeFile(const char *data);
void WriteBitcodeFile(const char *data, std::uint32_t size, std::string name);
bool HasXar();
}
}
}
