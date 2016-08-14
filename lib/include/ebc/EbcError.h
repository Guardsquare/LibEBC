#pragma once

#include <stdexcept>

namespace ebc {

class EbcError : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
  using std::runtime_error::what;
};
}
