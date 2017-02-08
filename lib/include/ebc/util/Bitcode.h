#pragma once

#include <cstdint>
#include <string>

namespace ebc {
namespace util {
namespace bitcode {

static constexpr auto BC_MAGIC = 0x0B17C0DEu;
static constexpr auto BC_CIGAM = 0xDEC0170Bu;
static constexpr auto IR_MAGIC = 0x4243C0DEu;
static constexpr auto IR_CIGAM = 0xDEC04342u;

/// Returns true if the first four bytes of the given data match the LLVM
/// bitcode magic number.
///
/// @param data Binary data representing a bitcode file.
///
/// @return True if the magic number BC 0xC0DE is found.
bool IsBitcode(const char *const data);

bool IsBitcode(std::uint32_t magic);

/// Returns true if the first four bytes fo the given file match the LLVM
/// bitcode magic number.
///
/// @param file Path to the file to check.
///
/// @return True if the magic number BC 0xC0DE is found.
bool IsBitcodeFile(std::string file);

/// Write data to file with given file name.
///
/// @param data Bitcode binary data.
/// @param size Bitcode binary data size.
/// @param fileName The desired filename for the bitcode file.
void WriteToFile(const char *data, std::uint32_t size, std::string file);
}
}
}
