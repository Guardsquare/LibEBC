#pragma once

#include <cstdint>
#include <string>

namespace ebc {
namespace util {
namespace bitcode {

/// Returns true if the first four bytes of the given data match the LLVM bitcode magic number.
bool IsBitcodeFile(const char *data);

/// Write data to file with given file name.
void WriteFile(const char *data, std::uint32_t size, std::string fileName);

/// Returns true if LibEBC is compiled with xar support.
bool HasXar();
}
}
}
