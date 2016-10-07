#include "ebc/BitcodeArchive.h"
#include "ebc/BitcodeContainer.h"
#include "ebc/BitcodeFile.h"
#include "ebc/BitcodeMetadata.h"
#include "ebc/BitcodeRetriever.h"
#include "ebc/util/Namer.h"

#include <tclap/CmdLine.h>

#include <iomanip>
#include <iostream>

using namespace ebc;
constexpr int WIDTH = 12;
constexpr int WIDTH_NESTED = 14;

static void printContainerInfo(const ebc::BitcodeContainer& bitcodeContainer) {
  std::cout << bitcodeContainer.GetBinaryMetadata().GetFileFormatName() << std::endl;
  std::cout << std::setw(WIDTH) << "File name:"
            << " " << bitcodeContainer.GetBinaryMetadata().GetFileName() << std::endl;
  std::cout << std::setw(WIDTH) << "Arch:"
            << " " << bitcodeContainer.GetBinaryMetadata().GetArch() << std::endl;
  std::cout << std::setw(WIDTH) << "UUID:"
            << " " << bitcodeContainer.GetBinaryMetadata().GetUUID() << std::endl;
}

static void printArchiveInfo(const ebc::BitcodeArchive& bitcodeArchive) {
  std::cout << std::setw(WIDTH) << "Dylibs:";
  for (const auto& dylib : bitcodeArchive.GetMetadata().GetDylibs()) {
    std::cout << " " << dylib;
  }
  std::cout << std::endl;

  std::cout << std::setw(WIDTH) << "Link opts:";
  for (const auto& option : bitcodeArchive.GetMetadata().GetLinkOptions()) {
    std::cout << " " << option;
  }
  std::cout << std::endl;
}

static void printBitcodeFiles(const ebc::BitcodeContainer& bitcodeContainer, bool extract) {
  for (auto& bitcodeFile : bitcodeContainer.GetBitcodeFiles(extract)) {
    std::cout << std::setw(WIDTH) << "Bitcode:"
              << " " << bitcodeFile.GetName() << std::endl;

    auto commands = bitcodeFile.GetCommands();
    if (!commands.empty()) {
      std::cout << std::setw(WIDTH_NESTED) << "Clang:";
      for (auto& command : commands) {
        std::cout << " " << command;
      }
      std::cout << std::endl;
    }
  }
}

static void printDetailled(const ebc::BitcodeContainer& bitcodeContainer, bool extract) {
  printContainerInfo(bitcodeContainer);
  if (bitcodeContainer.IsArchive()) {
    const auto& bitcodeArchive = static_cast<const BitcodeArchive&>(bitcodeContainer);
    printArchiveInfo(bitcodeArchive);
  }
  printBitcodeFiles(bitcodeContainer, extract);
}

static void printSimple(const ebc::BitcodeContainer& bitcodeContainer, bool extract) {
  std::cout << bitcodeContainer.GetBinaryMetadata().GetFileName() << " ("
            << bitcodeContainer.GetBinaryMetadata().GetArch() << "): ";

  for (auto& bitcodeFile : bitcodeContainer.GetBitcodeFiles(extract)) {
    std::cout << bitcodeFile.GetName() << " ";
  }
  std::cout << std::endl;
}

int main(int argc, char* argv[]) {
  try {
    // Command line arguments
    TCLAP::CmdLine cmd("Embedded Bitcode Tool", ' ', "1.0");
    TCLAP::SwitchArg extractArg("e", "extract", "Extract bitcode files", cmd, false);
    TCLAP::SwitchArg simpleArg("s", "simple", "Simple output, no details", cmd, false);

    TCLAP::ValueArg<std::string> archArg("a", "arch", "Limit to a single architecture", false, "", "string");
    cmd.add(archArg);

    TCLAP::ValueArg<std::string> directoryArg("d", "directory", "Directory for bitcode files", false, "", "string");
    cmd.add(directoryArg);

    TCLAP::ValueArg<std::string> prefixArg("p", "prefix", "Prefix for bitcode files", false, "", "string");
    cmd.add(prefixArg);

    TCLAP::UnlabeledValueArg<std::string> fileArg("File", "Library or object file", true, "", "file");
    cmd.add(fileArg);

    cmd.parse(argc, argv);

    const bool extract = extractArg.getValue() || directoryArg.isSet() || prefixArg.isSet();
    const bool simple = simpleArg.getValue();

    if (directoryArg.isSet()) {
      ebc::util::Namer::SetPath(directoryArg.getValue());
    }

    if (prefixArg.isSet()) {
      ebc::util::Namer::SetPrefix(prefixArg.getValue());
    }

    BitcodeRetriever bitcodeRetriever(fileArg.getValue());

    if (archArg.isSet()) {
      bitcodeRetriever.SetArch(archArg.getValue());
    }

    auto bitcodeContainers = bitcodeRetriever.GetBitcodeContainers();
    for (auto& bitcodeContainer : bitcodeContainers) {
      if (simple) {
        printSimple(*bitcodeContainer, extract);
      } else {
        printDetailled(*bitcodeContainer, extract);
      }
    }
  } catch (TCLAP::ArgException& e) {
    std::cerr << "Error: " << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  } catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "Error: exiting" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
