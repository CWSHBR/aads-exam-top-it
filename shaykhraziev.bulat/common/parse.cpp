#include "../common/parse.hpp"
#include <limits>

namespace
{
  bool isDigit(char symbol)
  {
    return (symbol >= '0') && (symbol <= '9');
  }

  bool isSpaceExceptNewline(char symbol)
  {
    return (symbol == ' ') || (symbol == '\t') || (symbol == '\v') || (symbol == '\f')
        || (symbol == '\r');
  }
}

size_t shaykhraziev::skipSpaces(const std::string& line, size_t position)
{
  while ((position < line.size()) && isSpaceExceptNewline(line[position]))
  {
    ++position;
  }
  return position;
}

bool shaykhraziev::parseSizeTPrefix(const std::string& line, size_t& value, size_t& position)
{
  size_t current = skipSpaces(line, position);
  if ((current >= line.size()) || !isDigit(line[current]))
  {
    return false;
  }
  size_t parsed = 0;
  while ((current < line.size()) && isDigit(line[current]))
  {
    const size_t digit = static_cast< size_t >(line[current] - '0');
    const size_t maxValue = std::numeric_limits< size_t >::max();
    if (parsed > ((maxValue - digit) / 10))
    {
      return false;
    }
    parsed = (parsed * 10) + digit;
    ++current;
  }
  value = parsed;
  position = current;
  return true;
}

bool shaykhraziev::readRestDescription(const std::string& line,
    size_t position,
    std::string& description)
{
  const size_t begin = skipSpaces(line, position);
  if (begin >= line.size())
  {
    return false;
  }
  description = line.substr(begin);
  return !description.empty();
}

bool shaykhraziev::startsWith(const std::string& text, const char* prefix)
{
  size_t position = 0;
  while (prefix[position] != '\0')
  {
    if ((position >= text.size()) || (text[position] != prefix[position]))
    {
      return false;
    }
    ++position;
  }
  return true;
}

bool shaykhraziev::parseFileArg(const std::string& argument,
    const char* prefix,
    std::string& filename)
{
  if (!startsWith(argument, prefix))
  {
    return false;
  }
  size_t prefixSize = 0;
  while (prefix[prefixSize] != '\0')
  {
    ++prefixSize;
  }
  filename = argument.substr(prefixSize);
  return !filename.empty();
}

bool shaykhraziev::hasTooManyU1Args(int argc)
{
  return argc > 3;
}

bool shaykhraziev::parseU1Args(int argc,
    char* argv[],
    bool& hasInput,
    std::string& input,
    bool& hasOutput,
    std::string& output)
{
  hasInput = false;
  hasOutput = false;
  input.clear();
  output.clear();
  if (hasTooManyU1Args(argc))
  {
    return false;
  }
  for (int i = 1; i < argc; ++i)
  {
    const std::string argument = argv[i];
    std::string filename;
    if (parseFileArg(argument, "in:", filename))
    {
      if (hasInput)
      {
        return false;
      }
      hasInput = true;
      input = filename;
    }
    else if (parseFileArg(argument, "out:", filename))
    {
      if (hasOutput)
      {
        return false;
      }
      hasOutput = true;
      output = filename;
    }
    else
    {
      return false;
    }
  }
  return true;
}

size_t shaykhraziev::hashSizeT(const size_t& value)
{
  return value;
}

bool shaykhraziev::equalSizeT(const size_t& left, const size_t& right)
{
  return left == right;
}
