#pragma once

#include <memory>
#include <string>
#include <vector>

namespace llvm {
namespace object {
class MachOObjectFile;
class ObjectFile;
}
}

namespace ebc {
class BitcodeArchive;
class BitcodeRetriever {
 public:
  BitcodeRetriever(std::string objectPath);

  std::vector<std::unique_ptr<BitcodeArchive>> GetBitcodeArchives();

 private:
  std::unique_ptr<BitcodeArchive> GetBitcodeArchiveFromMachO(llvm::object::MachOObjectFile* machOObjectFile) const;
  std::unique_ptr<BitcodeArchive> GetBitcodeArchiveFromObject(llvm::object::ObjectFile* objectFile) const;

  static std::string TripleToArch(unsigned arch);
  std::string _objectPath;
};
}
