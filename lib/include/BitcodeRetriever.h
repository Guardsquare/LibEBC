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
  std::string _objectPath;
};
}
