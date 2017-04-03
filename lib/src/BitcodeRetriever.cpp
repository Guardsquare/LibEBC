#include "ebc/BitcodeRetriever.h"

#include "ebc/BitcodeArchive.h"
#include "ebc/BitcodeContainer.h"
#include "ebc/BitcodeMetadata.h"
#include "ebc/EbcError.h"

#include "ebc/util/Xar.h"

#include "llvm/ADT/Triple.h"
#include "llvm/Object/ELFObjectFile.h"
#include "llvm/Object/MachO.h"
#include "llvm/Object/MachOUniversal.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/ErrorHandling.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>

using namespace llvm;
using namespace llvm::object;

namespace ebc {

/// Strip path from file name.
static std::string GetFileName(std::string fileName) {
  auto pos = fileName.rfind('/');
  return pos == std::string::npos ? fileName : fileName.substr(pos + 1);
}

/// Internal EBC error that extends LLVM's ErrorInfo class. We use this to have
/// the implementation return llvm::Expected<> objects.
class InternalEbcError : public llvm::ErrorInfo<InternalEbcError> {
 public:
  static char ID;

  InternalEbcError(std::string msg) : _msg(std::move(msg)) {}
  ~InternalEbcError(){};

  const std::string &getMessage() const {
    return _msg;
  }

  std::error_code convertToErrorCode() const override {
    llvm_unreachable("Not implemented");
  }

  void log(llvm::raw_ostream &OS) const override {
    OS << _msg;
  }

 private:
  std::string _msg;
};

char InternalEbcError::ID = 0;

/// Convert an LLVM error object to an EbcError. This works both for LLVM's
/// internal errors as well as for our custom InternalEbcError.
static EbcError ToEbcError(llvm::Error &&e) {
  std::string msg;
  handleAllErrors(std::move(e), [&msg](llvm::ErrorInfoBase &eib) { msg = eib.message(); });
  return EbcError(msg);
}

class BitcodeRetriever::Impl {
 public:
  typedef std::vector<std::unique_ptr<BitcodeContainer>> BitcodeContainers;

  Impl(std::string objectPath) : _objectPath(std::move(objectPath)) {}

  /// Set the architecture for which to retrieve bitcode. This is relevant to
  /// 'fat' object files containing multiple architectures. If no architecture
  /// is set, bitcode is retrieved for all present architectures.
  void SetArch(std::string arch) {
    _arch = std::move(arch);
  }

  /// Perform the actual bitcode retrieval. Depending on the type of the object
  /// file the resulting list contains plain bitcode containers or bitcode
  /// archives.
  ///
  /// @return A list of bitcode containers.
  BitcodeContainers GetBitcodeContainers() {
    auto binaryOrErr = createBinary(_objectPath);
    if (!binaryOrErr) {
      if (util::xar::IsXarFile(_objectPath)) {
        llvm::consumeError(binaryOrErr.takeError());
        auto bitcodeContainers = GetBitcodeContainersFromXar(_objectPath);
        if (!bitcodeContainers) {
          throw ToEbcError(bitcodeContainers.takeError());
        }
        return std::move(*bitcodeContainers);
      } else {
        throw ToEbcError(binaryOrErr.takeError());
      }
    }

    auto bitcodeContainers = GetBitcodeContainers(*binaryOrErr->getBinary());
    if (!bitcodeContainers) {
      throw ToEbcError(bitcodeContainers.takeError());
    }

    return std::move(*bitcodeContainers);
  }

 private:
  /// Helper method for determining whether the given architecture should be
  /// handled.
  ///
  /// @param arch The architecture.
  ///
  /// @return True if the architecture matches the set architure or when no
  /// architecture is set. False otherwise.
  bool processArch(const std::string &arch) const {
    return _arch.empty() ? true : (_arch == arch);
  }

  llvm::Expected<BitcodeContainers> GetBitcodeContainersFromXar(const std::string &xar) {
    auto bitcodeArchive = BitcodeArchive::BitcodeArchiveFromFile(xar);
    if (bitcodeArchive) {
      bitcodeArchive->GetBinaryMetadata().SetFileName(GetFileName(xar));
      auto bitcodeContainers = std::vector<std::unique_ptr<BitcodeContainer>>();
      bitcodeContainers.push_back(std::move(bitcodeArchive));
      return std::move(bitcodeContainers);
    }
    return llvm::make_error<InternalEbcError>("Could not create bitcode archive form Xar.");
  }

  /// Obtains all bitcode from an object. The method basically determines the
  /// kind of object and dispatches the actual work to the specialized method.
  ///
  /// @param binary The binary object.
  ///
  /// @return A list of bitcode containers.
  llvm::Expected<BitcodeContainers> GetBitcodeContainers(const llvm::object::Binary &binary) const {
    auto bitcodeContainers = std::vector<std::unique_ptr<BitcodeContainer>>();

    if (const auto *universalBinary = dyn_cast<MachOUniversalBinary>(&binary)) {
      // Iterate over all objects (i.e. architectures):
      for (auto object : universalBinary->objects()) {
        Expected<std::unique_ptr<MachOObjectFile>> machOObject = object.getAsObjectFile();
        if (machOObject) {
          auto container = GetBitcodeContainerFromMachO(machOObject->get());
          if (!container) {
            return container.takeError();
          }
          // Check for nullptr for when we only consider one architecture.
          if (container.get()) {
            bitcodeContainers.push_back(std::move(*container));
          }
          continue;
        } else {
          llvm::consumeError(machOObject.takeError());
        }

        Expected<std::unique_ptr<Archive>> archive = object.getAsArchive();
        if (archive) {
          auto containers = GetBitcodeContainersFromArchive(*archive->get());
          if (!containers) {
            return containers.takeError();
          }

          // We have to move all valid containers so we can move on to the next
          // architecture.
          bitcodeContainers.reserve(bitcodeContainers.size() + containers->size());
          std::copy_if(std::make_move_iterator(containers->begin()), std::make_move_iterator(containers->end()),
                       std::back_inserter(bitcodeContainers),
                       [](const auto &uniquePtr) { return uniquePtr != nullptr; });
          continue;
        } else {
          llvm::consumeError(archive.takeError());
        }
      }
    } else if (const auto machOObjectFile = dyn_cast<MachOObjectFile>(&binary)) {
      auto container = GetBitcodeContainerFromMachO(machOObjectFile);
      if (!container) {
        return container.takeError();
      }
      // Check for nullptr for when we only consider one architecture.
      if (container.get()) {
        bitcodeContainers.push_back(std::move(*container));
      }
    } else if (const auto object = dyn_cast<ObjectFile>(&binary)) {
      auto container = GetBitcodeContainerFromObject(object);
      if (!container) {
        return container.takeError();
      }
      // Check for nullptr for when we only consider one architecture.
      if (container.get()) {
        bitcodeContainers.push_back(std::move(*container));
      }
    } else if (const auto archive = dyn_cast<Archive>(&binary)) {
      // We can return early to prevent moving all containers in the vector.
      return GetBitcodeContainersFromArchive(*archive);
    } else {
      return llvm::make_error<InternalEbcError>("Unsupported binary");
    }

    return std::move(bitcodeContainers);
  }

  /// Obtains all bitcode from an object archive.
  ///
  /// @param archive The object archive.
  ///
  /// @return A list of bitcode containers.
  llvm::Expected<BitcodeContainers> GetBitcodeContainersFromArchive(const llvm::object::Archive &archive) const {
    Error err = Error::success();
    auto bitcodeContainers = std::vector<std::unique_ptr<BitcodeContainer>>();
    for (const auto &child : archive.children(err)) {
      if (err) {
        return std::move(err);
      }

      auto childOrErr = child.getAsBinary();
      if (!childOrErr) {
        return childOrErr.takeError();
      }
      auto containers = GetBitcodeContainers(*(*childOrErr));
      if (!containers) {
        return containers.takeError();
      }
      bitcodeContainers.reserve(bitcodeContainers.size() + containers->size());
      std::move(containers->begin(), containers->end(), std::back_inserter(bitcodeContainers));
    }

    return std::move(bitcodeContainers);
  }

  /// Reads bitcode from a Mach O object file.
  ///
  /// @param objectFile The Mach O object file.
  ///
  /// @return The bitcode container.
  llvm::Expected<std::unique_ptr<BitcodeContainer>> GetBitcodeContainerFromMachO(
      const llvm::object::MachOObjectFile *objectFile) const {
    // For MachO return the correct arch tripple.
    const std::string arch = objectFile->getArchTriple(nullptr).getArchName();
    if (!processArch(arch)) {
      return nullptr;
    }

    BitcodeContainer *bitcodeContainer = nullptr;

    const std::string name = objectFile->getFileFormatName().str();
    std::vector<std::string> commands;
    for (const SectionRef &section : objectFile->sections()) {
      // Get section name
      DataRefImpl dataRef = section.getRawDataRefImpl();
      StringRef segName = objectFile->getSectionFinalSegmentName(dataRef);

      if (segName == "__LLVM") {
        StringRef sectName;
        section.getName(sectName);
        if (sectName == "__bundle") {
          // Embedded bitcode in universal binary.
          auto data = GetSectionData(section);
          bitcodeContainer = new BitcodeArchive(data.first, data.second);
        } else if (sectName == "__bitcode") {
          // Embedded bitcode in single MachO object.
          auto data = GetSectionData(section);
          bitcodeContainer = new BitcodeContainer(data.first, data.second);
        } else if (sectName == "__cmd" || sectName == "__cmdline") {
          commands = GetCommands(section);
        }
      }
    }

    if (bitcodeContainer == nullptr) {
      return llvm::make_error<InternalEbcError>("No bitcode section in " + objectFile->getFileName().str());
    }

    // Set commands
    bitcodeContainer->SetCommands(commands);

    // Set binary metadata
    bitcodeContainer->GetBinaryMetadata().SetFileName(GetFileName(objectFile->getFileName()));
    bitcodeContainer->GetBinaryMetadata().SetFileFormatName(objectFile->getFileFormatName());
    bitcodeContainer->GetBinaryMetadata().SetArch(arch);
    bitcodeContainer->GetBinaryMetadata().SetUuid(objectFile->getUuid().data());

    return std::unique_ptr<BitcodeContainer>(bitcodeContainer);
  }

  /// Reads bitcode from a plain object file.
  ///
  /// @param objectFile The Mach O object file.
  ///
  /// @return The bitcode container.
  llvm::Expected<std::unique_ptr<BitcodeContainer>> GetBitcodeContainerFromObject(
      const llvm::object::ObjectFile *objectFile) const {
    const auto arch = llvm::Triple::getArchTypeName(static_cast<Triple::ArchType>(objectFile->getArch()));
    if (!processArch(arch)) {
      return nullptr;
    }

    BitcodeContainer *bitcodeContainer = nullptr;

    std::vector<std::string> commands;
    for (const SectionRef &section : objectFile->sections()) {
      StringRef sectName;
      section.getName(sectName);

      if (sectName == ".llvmbc") {
        auto data = GetSectionData(section);
        bitcodeContainer = new BitcodeContainer(data.first, data.second);
      } else if (sectName == ".llvmcmd") {
        commands = GetCommands(section);
      }
    }

    if (bitcodeContainer == nullptr) {
      return llvm::make_error<InternalEbcError>("No bitcode section in " + objectFile->getFileName().str());
    }

    // Set commands
    bitcodeContainer->SetCommands(commands);

    // Set binary metadata
    bitcodeContainer->GetBinaryMetadata().SetFileName(GetFileName(objectFile->getFileName()));
    bitcodeContainer->GetBinaryMetadata().SetFileFormatName(objectFile->getFileFormatName());
    bitcodeContainer->GetBinaryMetadata().SetArch(arch);

    return std::unique_ptr<BitcodeContainer>(bitcodeContainer);
  }

  /// Obtains data from a section.
  ///
  /// @param section The section from which the data should be obtained.
  ///
  /// @return A pair with the data and the size of the data.
  static std::pair<const char *, std::uint32_t> GetSectionData(const llvm::object::SectionRef &section) {
    StringRef bytesStr;
    section.getContents(bytesStr);
    const char *sect = reinterpret_cast<const char *>(bytesStr.data());
    uint32_t sect_size = bytesStr.size();
    return std::make_pair(sect, sect_size);
  }

  /// Obtains compiler commands from a section. It automatically parses the
  /// data into a vector.
  ///
  /// @param section The sectio from which to read the compiler commands.
  ///
  /// @return A vector of compiler commands.
  static std::vector<std::string> GetCommands(const llvm::object::SectionRef &section) {
    auto data = GetSectionData(section);
    const char *p = data.first;
    const char *end = data.first + data.second;

    // Create list of strings from commands separated by null bytes.
    std::vector<std::string> cmds;
    do {
      // Creating a string from p consumes data until next null byte.
      cmds.push_back(std::string(p));
      // Continue after the null byte.
      p += cmds.back().size() + 1;
    } while (p < end);

    return cmds;
  }

  std::string _objectPath;
  std::string _arch;
};

BitcodeRetriever::BitcodeRetriever(std::string objectPath) : _impl(std::make_unique<Impl>(std::move(objectPath))) {}
BitcodeRetriever::~BitcodeRetriever() = default;

void BitcodeRetriever::SetArch(std::string arch) {
  _impl->SetArch(std::move(arch));
}

std::vector<std::unique_ptr<BitcodeContainer>> BitcodeRetriever::GetBitcodeContainers() {
  return _impl->GetBitcodeContainers();
}

}  // namespace ebc
