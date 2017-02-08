#pragma once

#include <cstdint>
#include <map>
#include <string>

namespace ebc {
namespace util {
namespace macho {

static constexpr auto FAT_MAGIC = 0xCAFEBABEu;
static constexpr auto FAT_CIGAM = 0xBEBAFECAu;
static constexpr auto MH_MAGIC = 0xFEEDFACEu;
static constexpr auto MH_CIGAM = 0xCEFAEDFEu;
static constexpr auto MH_MAGIC_64 = 0xFEEDFACFu;
static constexpr auto MH_CIGAM_64 = 0xCFFAEDFEu;

bool IsMachO(std::uint32_t magic);
bool IsMachOFile(std::string file);
}
}
}
