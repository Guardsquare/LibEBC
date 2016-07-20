#pragma once

#include "BitcodeArchive.h"

#include <memory>
#include <string>
#include <vector>

namespace llvm {
namespace object {
class MachOObjectFile;
}
}

namespace ebc {
class BitcodeRetriever {
 public:
  BitcodeRetriever(std::string objectPath);

  std::vector<std::unique_ptr<BitcodeArchive>> GetBitcodeArchives();
  std::unique_ptr<BitcodeArchive> GetBitcodeArchive(llvm::object::MachOObjectFile* machOObjectFile) const;

 private:
  static std::string TripleToArch(unsigned arch);
  std::string _objectPath;
};
}
