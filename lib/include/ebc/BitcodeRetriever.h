#pragma once

#include <memory>
#include <string>
#include <vector>

namespace llvm {
namespace object {
class MachOObjectFile;
class ObjectFile;
class SectionRef;
}
}

namespace ebc {
class BitcodeContainer;
class BitcodeRetriever {
 public:
  BitcodeRetriever(std::string objectPath);

  std::vector<std::unique_ptr<BitcodeContainer>> GetBitcodeContainers();

 private:
  std::unique_ptr<BitcodeContainer> GetBitcodeContainerFromMachO(llvm::object::MachOObjectFile* machOObjectFile) const;
  std::unique_ptr<BitcodeContainer> GetBitcodeContainerFromObject(llvm::object::ObjectFile* objectFile) const;

  std::pair<const char*, std::uint32_t> GetSectionData(const llvm::object::SectionRef& section) const;
  std::vector<std::string> GetCommands(const llvm::object::SectionRef& section) const;
  void AddIfNotNull(BitcodeContainer* bitcodeContainer, std::vector<std::string> commands) const;

  static std::string TripleToArch(unsigned arch);
  std::string _objectPath;
};
}
