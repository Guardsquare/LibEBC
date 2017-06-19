#include "ebc/BitcodeArchive.h"
#include "ebc/BitcodeContainer.h"
#include "ebc/BitcodeMetadata.h"
#include "ebc/BitcodeRetriever.h"
#include "ebc/EmbeddedBitcode.h"
#include "ebc/EmbeddedFile.h"

#include <tclap/CmdLine.h>
#include <rang/rang.hpp>

#include <iomanip>
#include <iostream>

using namespace ebc;

static constexpr auto VERSION = "@VERSION_MAJOR@.@VERSION_MINOR@ (@GIT_COMMIT_HASH@)";
static constexpr int WIDTH = 12;
static constexpr int WIDTH_NESTED = 14;

static std::string filePrefix(const EmbeddedFile& file) {
  switch (file.GetType()) {
    case EmbeddedFile::Type::Bitcode: {
      auto bitcodeFile = static_cast<const EmbeddedBitcode&>(file);
      switch (bitcodeFile.GetBitcodeType()) {
        case BitcodeType::BitcodeWrapper:
          return "Wrapper:";
        case BitcodeType::IR:
          return "IR:";
        case BitcodeType::LTO:
          return "LTO:";
        default:
          return "Bitcode:";
      }
    }
    case EmbeddedFile::Type::Xar:
      return "Xar:";
    case EmbeddedFile::Type::LTO:
      return "LTO:";
    case EmbeddedFile::Type::Object:
      return "Object:";
    case EmbeddedFile::Type::File:
      return "File:";
    case EmbeddedFile::Type::Exports:
      return "Exports:";
  }
}

static void printContainerInfo(const BitcodeContainer& bitcodeContainer) {
  std::cout << rang::style::bold << bitcodeContainer.GetBinaryMetadata().GetFileFormatName() << rang::style::reset
            << std::endl;
  std::cout << std::setw(WIDTH) << "File name:"
            << " " << bitcodeContainer.GetBinaryMetadata().GetFileName() << std::endl;
  std::cout << std::setw(WIDTH) << "Arch:"
            << " " << bitcodeContainer.GetBinaryMetadata().GetArch() << std::endl;
  std::cout << std::setw(WIDTH) << "UUID:"
            << " " << bitcodeContainer.GetBinaryMetadata().GetUUID() << std::endl;
}

static void printArchiveInfo(const BitcodeArchive& bitcodeArchive) {
  std::cout << std::setw(WIDTH) << "Dylibs:";
  bool indent = false;
  for (const auto& dylib : bitcodeArchive.GetMetadata().GetDylibs()) {
    if (indent) {
      std::cout << std::setw(WIDTH) << " ";
    } else {
      indent = true;
    }
    std::cout << " " << dylib << std::endl;
  }

  std::cout << std::setw(WIDTH) << "Link opts:";
  for (const auto& option : bitcodeArchive.GetMetadata().GetLinkOptions()) {
    std::cout << " " << option;
  }
  std::cout << std::endl;
}

static void printEmbeddedFiles(const BitcodeContainer& bitcodeContainer, bool extract) {
  for (auto& embeddedFile : bitcodeContainer.GetEmbeddedFiles()) {
    std::cout << std::setw(WIDTH) << filePrefix(*embeddedFile) << " " << embeddedFile->GetName() << std::endl;

    if (!extract) {
      embeddedFile->Remove();
    }

    auto commands = embeddedFile->GetCommands();
    if (!commands.empty()) {
      std::cout << std::setw(WIDTH_NESTED) << "Clang:";
      for (auto& command : commands) {
        std::cout << " " << command;
      }
      std::cout << std::endl;
    }
  }
}

static void printDetailled(const BitcodeContainer& bitcodeContainer, bool extract) {
  printContainerInfo(bitcodeContainer);
  if (bitcodeContainer.IsArchive()) {
    const auto& bitcodeArchive = static_cast<const BitcodeArchive&>(bitcodeContainer);
    printArchiveInfo(bitcodeArchive);
  }
  printEmbeddedFiles(bitcodeContainer, extract);
}

static void printSimple(const BitcodeContainer& bitcodeContainer, bool extract) {
  std::cout << bitcodeContainer.GetBinaryMetadata().GetFileName() << " ("
            << bitcodeContainer.GetBinaryMetadata().GetArch() << "): ";

  for (auto& embeddedFile : bitcodeContainer.GetEmbeddedFiles()) {
    std::cout << embeddedFile->GetName() << " ";

    if (!extract) {
      embeddedFile->Remove();
    }
  }

  std::cout << std::endl;
}

int main(int argc, char* argv[]) {
  try {
    // Command line arguments
    TCLAP::CmdLine cmd("Embedded Bitcode Tool", ' ', VERSION, true);
    TCLAP::SwitchArg extractArg("e", "extract", "Extract bitcode files", cmd, false);
    TCLAP::SwitchArg simpleArg("s", "simple", "Simple output, no details", cmd, false);

    TCLAP::ValueArg<std::string> archArg("a", "arch", "Limit to a single architecture", false, "", "string");
    cmd.add(archArg);

    TCLAP::ValueArg<std::string> prefixArg("p", "prefix", "Prefix the files extracted by ebcutil", false, "", "string");
    cmd.add(prefixArg);

    TCLAP::UnlabeledValueArg<std::string> fileArg("File", "Binary file", true, "", "file");
    cmd.add(fileArg);

    cmd.parse(argc, argv);

    const bool extract = extractArg.getValue();

    BitcodeRetriever bitcodeRetriever(fileArg.getValue());

    if (archArg.isSet()) {
      bitcodeRetriever.SetArch(archArg.getValue());
    }

    auto bitcodeContainers = bitcodeRetriever.GetBitcodeContainers();
    if (bitcodeContainers.empty()) {
    }
    for (auto& bitcodeContainer : bitcodeContainers) {
      if (prefixArg.isSet()) {
        bitcodeContainer->SetPrefix(prefixArg.getValue());
      }

      if (simpleArg.getValue()) {
        printSimple(*bitcodeContainer, extract);
      } else {
        printDetailled(*bitcodeContainer, extract);
      }
    }
  } catch (TCLAP::ArgException& e) {
    std::cerr << rang::fg::red << "Error: " << rang::fg::reset << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  } catch (std::exception& e) {
    std::cerr << rang::fg::red << "Error: " << rang::fg::reset << e.what() << std::endl;
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << rang::fg::red << "Error: " << rang::fg::reset << "exiting" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
