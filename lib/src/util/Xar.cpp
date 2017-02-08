#include "ebc/util/Xar.h"

#include "ebc/Config.h"
#include "ebc/util/UUID.h"

#ifdef HAVE_LIBXAR
extern "C" {
#include <xar/xar.h>
}
#endif

#include <cstring>
#include <iostream>

namespace ebc {
namespace util {
namespace xar {

bool IsXarFile(std::string file) {
#ifdef HAVE_LIBXAR
  xar_t x = xar_open(file.c_str(), READ);

  if (x == nullptr) return false;

  xar_close(x);
  return true;
#else
  return false;
#endif
}

bool WriteTOC(std::string xarFile, std::string tocFile) {
#ifdef HAVE_LIBXAR
  xar_t x = xar_open(xarFile.c_str(), READ);

  if (x == nullptr) return false;

  xar_serialize(x, tocFile.c_str());
  xar_close(x);

  return true;
#else
  return false;
#endif
}

std::map<std::string, std::string> Extract(std::string file, std::string prefix) {
  std::map<std::string, std::string> files;
#ifdef HAVE_LIBXAR
  xar_t x;
  xar_iter_t xi;
  xar_file_t xf;
  xar_stream xs;
  char buffer[8192];

  x = xar_open(file.c_str(), READ);
  if (x == nullptr) {
    std::cerr << "Could not open xar archive" << std::endl;
  }

  xi = xar_iter_new();
  if (xi == nullptr) {
    xar_close(x);
    std::cerr << "Could not read xar archive" << std::endl;
  }

  for (xf = xar_file_first(x, xi); xf != nullptr; xf = xar_file_next(xi)) {
    char *path = xar_get_path(xf);
    const char *type;
    xar_prop_get(xf, "type", &type);

    if (type == nullptr) {
      std::cerr << "File has no type" << std::endl;
      free(path);
      continue;
    }

    if (std::strcmp(type, "file") != 0) {
      free(path);
      continue;
    }

    if (xar_extract_tostream_init(x, xf, &xs) != XAR_STREAM_OK) {
      std::cerr << "Error initializing stream" << std::endl;
      free(path);
      continue;
    }

    const std::string fileName = prefix + util::uuid::UuidToString(util::uuid::GenerateUUID());

    // Write bitcode to file
    std::FILE *output = std::fopen(fileName.c_str(), "wb");
    if (output == nullptr) {
      std::cerr << "Error opening output file" << std::endl;
      continue;
    }

    xs.avail_out = sizeof(buffer);
    xs.next_out = buffer;

    int32_t ret;
    while ((ret = xar_extract_tostream(&xs)) != XAR_STREAM_END) {
      if (ret == XAR_STREAM_ERR) {
        std::cerr << "Error extracting stream" << std::endl;
        break;
      }
      std::fwrite(buffer, sizeof(char), sizeof(buffer) - xs.avail_out, output);

      xs.avail_out = sizeof(buffer);
      xs.next_out = buffer;
    }

    if (xar_extract_tostream_end(&xs) != XAR_STREAM_OK) {
      std::cerr << "Error ending stream" << std::endl;
    }

    std::fclose(output);

    // Add to list of extracted files
    files[std::string(path)] = fileName;

    free(path);
  }

  xar_iter_free(xi);
  xar_close(x);
#endif
  return files;
}

bool HasXar() {
#ifdef HAVE_LIBXAR
  return true;
#else
  return false;
#endif
}
}
}
}
