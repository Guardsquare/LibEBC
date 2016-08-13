#include "ebc/BitcodeArchive.h"
#include "ebc/BitcodeContainer.h"
#include "ebc/BitcodeMetadata.h"
#include "ebc/BitcodeRetriever.h"

#include <iomanip>
#include <iostream>

using namespace ebc;
constexpr int WIDTH = 12;
constexpr int WIDTH_NESTED = 14;

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: ebcutil <binary>" << std::endl;
  }

  BitcodeRetriever bitcodeRetriever(argv[1]);
  for (auto& bitcodeContainer : bitcodeRetriever.GetBitcodeContainers()) {
    std::cout << bitcodeContainer->GetBinaryMetadata().GetFileFormatName() << std::endl;
    std::cout << std::setw(WIDTH) << "File name:"
              << " " << bitcodeContainer->GetBinaryMetadata().GetFileName() << std::endl;
    std::cout << std::setw(WIDTH) << "Arch:"
              << " " << bitcodeContainer->GetBinaryMetadata().GetArch() << std::endl;
    std::cout << std::setw(WIDTH) << "UUID:"
              << " " << bitcodeContainer->GetBinaryMetadata().GetUUID() << std::endl;

    if (bitcodeContainer->IsArchive()) {
      auto bitcodeArchive = static_cast<BitcodeArchive*>(bitcodeContainer.get());

      std::cout << std::setw(WIDTH) << "Dylibs:";
      for (auto& dylib : bitcodeArchive->GetMetadata().GetDylibs()) {
        std::cout << " " << dylib;
      }
      std::cout << std::endl;

      std::cout << std::setw(WIDTH) << "Link opts:";
      for (auto& option : bitcodeArchive->GetMetadata().GetLinkOptions()) {
        std::cout << " " << option;
      }
      std::cout << std::endl;
    }

    for (auto& bitcodeFile : bitcodeContainer->GetBitcodeFiles()) {
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
}
