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

/// Retrieves bitcode form an object file.
class BitcodeRetriever {
 public:
  /// Creates an instance of the bitcode retriever for the given object file.
  ///
  /// @param objectPath The file from which to retrieve bitcode.
  BitcodeRetriever(std::string objectPath);

  /// Set the architecture for which to retrieve bitcode. This is relevant to
  /// 'fat' object files containing multiple architectures. If no architecture
  /// is set, bitcode is retrieved for all present architectures.
  ///
  /// @param arch The architecture.
  void SetArch(std::string arch);

  /// Perform the actual bitcode retrieval. Depending on the type of the object
  /// file the resulting list contains plain bitcode containers or bitcode
  /// archives.
  ///
  /// @return A list of bitcode containers.
  std::vector<std::unique_ptr<BitcodeContainer>> GetBitcodeContainers();

 private:
  /// Helper method for determining whether the given architecture should be
  /// handled.
  ///
  /// @param arch The architecture.
  ///
  /// @return True if the architecture matches the set architure or when no
  /// architecture is set. False otherwise.
  bool processArch(std::string arch) const;

  /// Obtains all bitcode from an object. The method basically determines the
  /// kind of object and dispatches the actual work to the specialized method.
  ///
  /// @param binary The binary object.
  ///
  /// @return A list of bitcode containers.
  std::vector<std::unique_ptr<BitcodeContainer>> GetBitcodeContainers(const llvm::object::Binary& binary) const;

  /// Obtains all bitcode from an object archive.
  ///
  /// @param archive The object archive.
  ///
  /// @return A list of bitcode containers.
  std::vector<std::unique_ptr<BitcodeContainer>> GetBitcodeContainersFromArchive(
      const llvm::object::Archive& archive) const;

  /// Reads bitcode from a Mach O object file.
  ///
  /// @param objectFile The Mach O object file.
  ///
  /// @return The bitcode container.
  std::unique_ptr<BitcodeContainer> GetBitcodeContainerFromMachO(const llvm::object::MachOObjectFile* objectFile) const;

  /// Reads bitcode from a plain object file.
  ///
  /// @param objectFile The Mach O object file.
  ///
  /// @return The bitcode container.
  std::unique_ptr<BitcodeContainer> GetBitcodeContainerFromObject(const llvm::object::ObjectFile* objectFile) const;

  /// Obtains data from a section.
  ///
  /// @param section The section from which the data should be obtained.
  ///
  /// @return A pair with the data and the size of the data.
  static std::pair<const char*, std::uint32_t> GetSectionData(const llvm::object::SectionRef& section);

  /// Obtains compiler commands from a section. It automatically parses the
  /// data into a vector.
  ///
  /// @param section The sectio from which to read the compiler commands.
  ///
  /// @return A vector of compiler commands.
  static std::vector<std::string> GetCommands(const llvm::object::SectionRef& section);

  std::string _objectPath;
  std::string _arch;
};
}
