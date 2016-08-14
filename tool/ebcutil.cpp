#include "ebc/BitcodeArchive.h"
#include "ebc/BitcodeContainer.h"
#include "ebc/BitcodeFile.h"
#include "ebc/BitcodeMetadata.h"
#include "ebc/BitcodeRetriever.h"

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

static void printBitcodeFiles(const ebc::BitcodeContainer& bitcodeContainer) {
  for (auto& bitcodeFile : bitcodeContainer.GetBitcodeFiles()) {
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

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: ebcutil <binary>" << std::endl;
    return EXIT_FAILURE;
  }

  try {
    BitcodeRetriever bitcodeRetriever(argv[1]);
    auto bitcodeContainers = bitcodeRetriever.GetBitcodeContainers();
    for (auto& bitcodeContainer : bitcodeContainers) {
      printContainerInfo(*bitcodeContainer);
      if (bitcodeContainer->IsArchive()) {
        auto bitcodeArchive = static_cast<BitcodeArchive*>(bitcodeContainer.get());
        printArchiveInfo(*bitcodeArchive);
      }
      printBitcodeFiles(*bitcodeContainer);
    }
  } catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
