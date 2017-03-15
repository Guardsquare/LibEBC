#pragma once

#include <memory>
#include <string>

namespace ebc {
class EmbeddedFile;
class EmbeddedFileFactory {
 public:
  static std::unique_ptr<EmbeddedFile> CreateEmbeddedFile(std::string file);
  static std::unique_ptr<EmbeddedFile> CreateEmbeddedFile(std::string file, std::string fileType);
};
}
