#include "ebc/BitcodeMetadata.h"

#include "catch.hpp"

#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include <vector>

using namespace ebc;

static constexpr auto xar_xml_empty =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<xar>"
    "</xar>";

static constexpr auto xar_xml =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    "<xar>"
    " <subdoc subdoc_name=\"Ld\">"
    "  <version>1.0</version>"
    "  <architecture>x86_64</architecture>"
    "  <platform>MacOSX</platform>"
    "  <hide-symbols>1</hide-symbols>"
    "  <sdkversion>10.11.0</sdkversion>"
    "  <dylibs>"
    "   <lib>{SDKPATH}/usr/lib/libSystem.B.dylib</lib>"
    "  </dylibs>"
    "  <link-options>"
    "   <option>-execute</option>"
    "   <option>-macosx_version_min</option>"
    "   <option>10.11.0</option>"
    "   <option>-e</option>"
    "   <option>_main</option>"
    "   <option>-executable_path</option>"
    "   <option>build/x86_64.o</option>"
    "  </link-options>"
    " </subdoc>"
    " <toc>"
    "  <checksum style=\"sha1\">"
    "   <size>20</size>"
    "   <offset>0</offset>"
    "  </checksum>"
    "  <creation-time>2016-06-20T18:53:42</creation-time>"
    "  <file id=\"1\">"
    "   <name>1</name>"
    "   <type>file</type>"
    "   <data>"
    "    <archived-checksum style=\"sha1\">96373cb0f493ca95a458af35fc819e5c04734fed</archived-checksum>"
    "    <extracted-checksum style=\"sha1\">96373cb0f493ca95a458af35fc819e5c04734fed</extracted-checksum>"
    "    <size>1952</size>"
    "    <offset>20</offset>"
    "    <encoding style=\"application/octet-stream\"/>"
    "    <length>1952</length>"
    "   </data>"
    "   <file-type>Bitcode</file-type>"
    "   <clang>"
    "    <cmd>-triple</cmd>"
    "    <cmd>x86_64-apple-macosx10.11.0</cmd>"
    "    <cmd>-emit-obj</cmd>"
    "    <cmd>-disable-llvm-optzns</cmd>"
    "   </clang>"
    "  </file>"
    "  <file id=\"2\">"
    "   <name>2</name>"
    "   <type>file</type>"
    "   <data>"
    "    <archived-checksum style=\"sha1\">62aa5b87fe61627b8cc293f599c81b9728ca0bef</archived-checksum>"
    "    <extracted-checksum style=\"sha1\">62aa5b87fe61627b8cc293f599c81b9728ca0bef</extracted-checksum>"
    "    <size>1392</size>"
    "    <offset>1972</offset>"
    "    <encoding style=\"application/octet-stream\"/>"
    "    <length>1392</length>"
    "   </data>"
    "   <file-type>Bitcode</file-type>"
    "   <swift>"
    "    <cmd>-bogus</cmd>"
    "    <cmd>-cmd</cmd>"
    "   </swift>"
    "  </file>"
    " </toc>"
    "</xar>";

TEST_CASE("GetXml", "[BitcodeMetadata]") {
  const BitcodeMetadata metadata(xar_xml);
  REQUIRE(xar_xml == metadata.GetXml());
}

TEST_CASE("GetDylibs", "[BitcodeMetadata]") {
  const BitcodeMetadata metadata(xar_xml);
  const std::vector<std::string> dylibs = {"{SDKPATH}/usr/lib/libSystem.B.dylib"};
  REQUIRE(dylibs == metadata.GetDylibs());
}

TEST_CASE("GetDylibs (empty)", "[BitcodeMetadata]") {
  const BitcodeMetadata metadata(xar_xml_empty);
  const std::vector<std::string> empty;
  REQUIRE(empty == metadata.GetDylibs());
}

TEST_CASE("GetLinkOptions", "[BitcodeMetadata]") {
  const BitcodeMetadata metadata(xar_xml);
  const std::vector<std::string> options = {"-execute", "-macosx_version_min", "10.11.0",       "-e",
                                            "_main",    "-executable_path",    "build/x86_64.o"};
  REQUIRE(options == metadata.GetLinkOptions());
}

TEST_CASE("GetLinkOptions (empty)", "[BitcodeMetadata]") {
  const BitcodeMetadata metadata(xar_xml_empty);
  const std::vector<std::string> empty;
  REQUIRE(empty == metadata.GetLinkOptions());
}

TEST_CASE("GetCommands", "[BitcodeMetadata]") {
  const BitcodeMetadata metadata(xar_xml_empty);
  const std::vector<std::string> empty;
  REQUIRE(empty == metadata.GetClangCommands("bogusFileName"));
  REQUIRE(empty == metadata.GetSwiftCommands("bogusFileName"));
}

TEST_CASE("GetClangCommands", "[BitcodeMetadata]") {
  const BitcodeMetadata metadata(xar_xml);
  const std::vector<std::string> commands = {"-triple", "x86_64-apple-macosx10.11.0", "-emit-obj",
                                             "-disable-llvm-optzns"};
  REQUIRE(commands == metadata.GetClangCommands("1"));
}

TEST_CASE("GetSwiftCommands", "[BitcodeMetadata]") {
  const BitcodeMetadata metadata(xar_xml);
  const std::vector<std::string> commands = {"-bogus", "-cmd"};
  REQUIRE(commands == metadata.GetSwiftCommands("2"));
}

TEST_CASE("GetVersion", "[BitcodeMetadata]") {
  const BitcodeMetadata metadata(xar_xml);
  REQUIRE("1.0" == metadata.GetVersion());
}

TEST_CASE("GetPlatform", "[BitcodeMetadata]") {
  const BitcodeMetadata metadata(xar_xml);
  REQUIRE("MacOSX" == metadata.GetPlatform());
}

TEST_CASE("GetArchitecture", "[BitcodeMetadata]") {
  const BitcodeMetadata metadata(xar_xml);
  REQUIRE("x86_64" == metadata.GetArchitecture());
}

TEST_CASE("GetSdkVersion", "[BitcodeMetadata]") {
  const BitcodeMetadata metadata(xar_xml);
  REQUIRE("10.11.0" == metadata.GetSdkVersion());
}

TEST_CASE("GetHideSymbols", "[BitcodeMetadata]") {
  const BitcodeMetadata metadata(xar_xml);
  REQUIRE("1" == metadata.GetHideSymbols());
}
