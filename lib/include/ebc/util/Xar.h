#pragma once

#include <map>
#include <string>

namespace ebc {
namespace util {
namespace xar {

/// Returns true if the given file is a xar archive.
///
/// @param file Path to the file.
///
/// @return True if and only if the given file is a xar archive. Always false if build without xar support.
bool IsXarFile(std::string file);

/// Write the ToC (Table of Content) of the given xar archive to disk.
///
/// @param xarFile The path to the xar archive.
/// @param tocFile The desired path for the ToC file.
///
/// @return True if the toc was written to file. Always false if build without xar support.
bool WriteTOC(std::string xarFile, std::string tocFile);

/// Extract the files in the archive.
///
/// @param file Patht to the xar xarchive.
///
/// @return A map with the original filename and a generated unique (UUID) filename.
std::map<std::string, std::string> Extract(std::string file, std::string prefix = "");

/// Returns true if LibEBC is compiled with xar support.
///
/// @return True if and only if XAR support is enabled.
bool HasXar();
}
}
}
