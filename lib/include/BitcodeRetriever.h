#pragma once

#include "BitcodeArchive.h"

#include <string>
#include <vector>

namespace ebc {
class BitcodeRetriever {
 public:
  BitcodeRetriever(std::string objectPath);

  std::vector<BitcodeArchive> GetBitcodeArchives();

 private:
  static std::string TripleToArch(unsigned arch);
  std::string _objectPath;
};
}
