#pragma once

#include <memory>
#include <string>
#include <vector>

namespace llvm {
namespace object {
class Archive;
class Binary;
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
  static std::vector<std::unique_ptr<BitcodeContainer>> GetBitcodeContainers(llvm::object::Binary& binary);
  static std::vector<std::unique_ptr<BitcodeContainer>> GetBitcodeContainersFromArchive(llvm::object::Archive& archive);
  static std::unique_ptr<BitcodeContainer> GetBitcodeContainerFromMachO(llvm::object::MachOObjectFile* objectFile);
  static std::unique_ptr<BitcodeContainer> GetBitcodeContainerFromObject(llvm::object::ObjectFile* objectFile);

  static std::pair<const char*, std::uint32_t> GetSectionData(const llvm::object::SectionRef& section);
  static std::vector<std::string> GetCommands(const llvm::object::SectionRef& section);

  std::string _objectPath;
};
}
