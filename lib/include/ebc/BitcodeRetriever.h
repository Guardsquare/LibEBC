#pragma once

#include <memory>
#include <string>
#include <vector>

namespace ebc {
class BitcodeContainer;

/// Retrieves bitcode form an object file.
class BitcodeRetriever {
 public:
  struct BitcodeInfo {
    std::string arch;
    std::unique_ptr<BitcodeContainer> bitcodeContainer;
    BitcodeInfo(std::string arch, std::unique_ptr<BitcodeContainer> bitcodeContainer)
        : arch(std::move(arch)), bitcodeContainer(std::move(bitcodeContainer)) {}
  };

  /// Creates an instance of the bitcode retriever for the given object file.
  ///
  /// @param objectPath The file from which to retrieve bitcode.
  BitcodeRetriever(std::string objectPath);
  ~BitcodeRetriever();

  /// Perform the actual bitcode retrieval. Depending on the type of the object
  /// file the resulting list contains plain bitcode containers or bitcode
  /// archives.
  ///
  /// @return A list of bitcode containers.
  std::vector<BitcodeInfo> GetBitcodeInfo();

 private:
  class Impl;
  std::unique_ptr<Impl> _impl;
};
}
