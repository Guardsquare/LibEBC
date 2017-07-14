#pragma once

#include <memory>
#include <string>
#include <vector>

namespace ebc {
class BitcodeContainer;

/// Retrieves bitcode form an object file.
class BitcodeRetriever {
 public:
  /// Creates an instance of the bitcode retriever for the given object file.
  ///
  /// @param objectPath The file from which to retrieve bitcode.
  BitcodeRetriever(std::string objectPath);
  ~BitcodeRetriever();

  /// Set the architectures for which to retrieve bitcode. This is relevant to
  /// 'fat' object files containing multiple architectures. If no architecture
  /// is set, bitcode is retrieved for all present architectures.
  ///
  /// @param arch The architecture.
  void SetArchs(std::vector<std::string> archs);

  /// Perform the actual bitcode retrieval. Depending on the type of the object
  /// file the resulting list contains plain bitcode containers or bitcode
  /// archives.
  ///
  /// @return A list of bitcode containers.
  std::vector<std::unique_ptr<BitcodeContainer>> GetBitcodeContainers();

 private:
  class Impl;
  std::unique_ptr<Impl> _impl;
};
}
