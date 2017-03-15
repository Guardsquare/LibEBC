#pragma once

#include <memory>
#include <string>
#include <vector>

struct _xmlDoc;
struct _xmlNode;

namespace ebc {

/// Abstraction of bitcode meta data associated with a BitcodeArchive. The data
/// comes from the table of content of the embedded XAR archive.
class BitcodeMetadata {
 public:
  BitcodeMetadata(std::string xml);
  BitcodeMetadata(const BitcodeMetadata& bitcodeTOC) = delete;

  ~BitcodeMetadata();

  /// Returns the meta data as XML string.
  ///
  /// @return The XML data as a string.
  const std::string& GetXml() const;

  std::string GetVersion() const;
  std::string GetArchitecture() const;
  std::string GetHideSymbols() const;
  std::string GetPlatform() const;
  std::string GetSdkVersion() const;

  /// Get dynamic libraries used for linking.
  ///
  /// @return A vector of strings with the name of the dylibs this binary was
  ///         linked against.
  std::vector<std::string> GetDylibs() const;

  /// Get weak dynamic libraries used for linking.
  ///
  /// @return A vector of strings with the name of the dylibs this binary was
  ///         linked against.
  std::vector<std::string> GetWeakDylibs() const;

  /// Get options passed to linker.
  ///
  /// @return A list of strings with the options passed to the linker.
  std::vector<std::string> GetLinkOptions() const;

  /// Get the commands passed to clang for the given file.
  ///
  /// @param fileName The input file passed to the clang compiler.
  ///
  /// @return The comands passed to the clang front end for the given file.
  std::vector<std::string> GetClangCommands(std::string fileName) const;

  /// Get the commands passed to swiftc for the given file.
  ///
  /// @param fileName The input file passed to the swift compiler.
  ///
  /// @return The comands passed to the swift front end for the given file.
  std::vector<std::string> GetSwiftCommands(std::string fileName) const;

  /// Get the file type for the given file.
  ///
  /// @param fileName The file.
  ///
  /// @return The file type of the given file.
  std::string GetFileType(std::string fileName) const;

 private:
  std::vector<std::string> GetCommands(std::string fileName, std::string nodeName) const;

  std::string _xml;
  _xmlDoc* _doc;
  _xmlNode* _root;
};
}
