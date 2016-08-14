#pragma once

#include <array>
#include <cstdio>
#include <string>

namespace ebc {
namespace util {
namespace uuid {

/// Convert UUID to canonical format with hyphens.
std::string UuidToString(std::array<std::uint8_t, 16> uuid);
}
}
}
