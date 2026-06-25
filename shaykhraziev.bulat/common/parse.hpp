#ifndef PARSE_HPP
#define PARSE_HPP

#include <cstddef>
#include <string>

namespace shaykhraziev
{
  size_t skipSpaces(const std::string& line, size_t position);
  bool parseSizeTPrefix(const std::string& line, size_t& value, size_t& position);
  bool readRestDescription(const std::string& line, size_t position, std::string& description);
  bool startsWith(const std::string& text, const char* prefix);
  bool parseFileArg(const std::string& argument, const char* prefix, std::string& filename);
  bool hasTooManyU1Args(int argc);
  bool parseU1Args(int argc,
      char* argv[],
      bool& hasInput,
      std::string& input,
      bool& hasOutput,
      std::string& output);
  size_t hashSizeT(const size_t& value);
  bool equalSizeT(const size_t& left, const size_t& right);
}

#endif
