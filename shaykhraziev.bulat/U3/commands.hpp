#ifndef U3_COMMANDS_HPP
#define U3_COMMANDS_HPP

#include <iosfwd>
#include <string>
#include "program.hpp"

namespace shaykhraziev
{
  bool executeAnons(U3Storage& storage, std::ostream& output);
  bool executeDesc(U3Storage& storage, const std::string& line, std::ostream& output);
  bool executeRedesc(U3Storage& storage, const std::string& line);
  bool executeMeets(U3Storage& storage, const std::string& line, std::ostream& output);
  bool executeLess(U3Storage& storage, const std::string& line, std::ostream& output);
  bool executeGreater(U3Storage& storage, const std::string& line, std::ostream& output);
  bool executeCommons(U3Storage& storage, const std::string& line, std::ostream& output);
  bool executeDeanon(U3Storage& storage, const std::string& line);
  bool executeOutPersons(U3Storage& storage, const std::string& line, std::ostream& output);
  bool executeCommand(U3Storage& storage, const std::string& line, std::ostream& output);
  void runCommandLoop(U3Storage& storage, std::istream& input, std::ostream& output);
}

#endif
