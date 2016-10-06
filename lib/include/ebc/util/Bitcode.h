#pragma once

#include <cstdint>
#include <string>

namespace ebc {
namespace util {
namespace bitcode {

/// Returns true if the first four bytes of the given data match the LLVM
/// bitcode magic number.
///
/// @param data Binary data representing a bitcode file.
///
/// @return True if the magic number B17C is found.
bool IsBitcodeFile(const char *data);

/// Write data to file with given file name.
///
/// @param data Bitcode binary data.
/// @param size Bitcode binary data size.
/// @param fileName The desired filename for the bitcode file.
void WriteFile(const char *data, std::uint32_t size, std::string fileName);

/// Returns true if LibEBC is compiled with xar support.
///
/// @return True if and only if XAR support is enabled.
bool HasXar();
}
}
}
