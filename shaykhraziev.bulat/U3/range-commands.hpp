#ifndef U3_RANGE_COMMANDS_HPP
#define U3_RANGE_COMMANDS_HPP

#include <iosfwd>
#include <string>
#include "program.hpp"

namespace shaykhraziev
{
  DateRange* getCurrentRange(U3Storage& storage);
  void printCurrentRange(U3Storage& storage, std::ostream& output);
  bool executeRange(U3Storage& storage, const std::string& line, std::ostream& output);
  bool executeAfter(U3Storage& storage, const std::string& line);
  bool executeBefore(U3Storage& storage, const std::string& line);
  bool executePopRange(U3Storage& storage, const std::string& line);
}

#endif
