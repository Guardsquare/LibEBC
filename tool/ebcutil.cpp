#include "BitcodeRetriever.h"

#include <iomanip>
#include <iostream>

using namespace ebc;
constexpr int width = 12;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: ebcutil <macho> [options]" << std::endl;
  }

  BitcodeRetriever bitcodeRetriever(argv[1]);
  for (auto& bitcodeArchive : bitcodeRetriever.GetBitcodeArchives()) {
    std::cout << bitcodeArchive.GetName() << std::endl;
    std::cout << std::setw(width) << "UUID:"
              << " " << bitcodeArchive.GetUUID() << std::endl;

    std::cout << std::setw(width) << "Dylibs:";
    for (auto& dylib : bitcodeArchive.GetMetadata().GetDylibs()) {
      std::cout << " " << dylib;
    }
    std::cout << std::endl;

    std::cout << std::setw(width) << "Link opts:";
    for (auto& option : bitcodeArchive.GetMetadata().GetLinkOptions()) {
      std::cout << " " << option;
    }
    std::cout << std::endl;

    for (auto& bitcodeFile : bitcodeArchive.GetBitcodeFiles()) {
      std::cout << std::setw(width) << "File:"
                << " " << bitcodeFile.GetName();

      std::cout << " (Clang:";
      for (auto& clangCommand : bitcodeFile.GetClangCommands()) {
        std::cout << " " << clangCommand;
      }
      std::cout << ")" << std::endl;
    }

    std::cout << std::endl;
  }
}
