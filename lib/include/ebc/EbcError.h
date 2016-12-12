#pragma once

#include <stdexcept>

namespace ebc {

/// Exception thrown by LibEBC.
class EbcError : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
  using std::runtime_error::what;
};

/// Exception thrown when no bitcode section is found.
class NoBitcodeError : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
  using std::runtime_error::what;
};
}
