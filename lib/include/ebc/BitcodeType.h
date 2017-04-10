#pragma once

namespace ebc {

enum class BitcodeType {
  Bitcode,
  BitcodeWrapper,
  IR,
  LTO,
  Unknown,
};
}
