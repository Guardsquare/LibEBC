#pragma once

#include "ebc/EmbeddedFile.h"

#include <string>

namespace ebc {
class EmbeddedObject : public EmbeddedFile {
 public:
  EmbeddedObject(std::string file) : EmbeddedFile(std::move(file), EmbeddedFile::Type::Object) {}
  ~EmbeddedObject() = default;
};
}
