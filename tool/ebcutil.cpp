#include "BitcodeRetriever.h"

#include <iomanip>
#include <iostream>

using namespace ebc;
constexpr int WIDTH = 12;
constexpr int WIDTH_NESTED = 14;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: ebcutil <macho> [options]" << std::endl;
  }

  BitcodeRetriever bitcodeRetriever(argv[1]);
  for (auto& bitcodeArchive : bitcodeRetriever.GetBitcodeArchives()) {
    std::cout << bitcodeArchive.GetName() << std::endl;
    std::cout << std::setw(WIDTH) << "UUID:"
              << " " << bitcodeArchive.GetUUID() << std::endl;
    std::cout << std::setw(WIDTH) << "Arch:"
              << " " << bitcodeArchive.GetArch() << std::endl;

    std::cout << std::setw(WIDTH) << "Dylibs:";
    for (auto& dylib : bitcodeArchive.GetMetadata().GetDylibs()) {
      std::cout << " " << dylib;
    }
    std::cout << std::endl;

    std::cout << std::setw(WIDTH) << "Link opts:";
    for (auto& option : bitcodeArchive.GetMetadata().GetLinkOptions()) {
      std::cout << " " << option;
    }
    std::cout << std::endl;

    for (auto& bitcodeFile : bitcodeArchive.GetBitcodeFiles()) {
      std::cout << std::setw(WIDTH) << "File:"
                << " " << bitcodeFile.GetName() << std::endl;

      auto clangCommands = bitcodeFile.GetClangCommands();
      if (!clangCommands.empty()) {
        std::cout << std::setw(WIDTH_NESTED) << "Clang:";
        for (auto& clangCommand : bitcodeFile.GetClangCommands()) {
          std::cout << " " << clangCommand;
        }
        std::cout << std::endl;
      }

      auto swiftCommands = bitcodeFile.GetSwiftCommands();
      if (!swiftCommands.empty()) {
        std::cout << std::setw(WIDTH_NESTED) << "Swift:";
        for (auto& swiftCommand : bitcodeFile.GetSwiftCommands()) {
          std::cout << " " << swiftCommand;
        }
        std::cout << std::endl;
      }
    }

    std::cout << std::endl;
  }
}
