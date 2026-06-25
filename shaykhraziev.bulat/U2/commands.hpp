#ifndef U2_COMMANDS_HPP
#define U2_COMMANDS_HPP

#include <iosfwd>
#include <string>
#include "program.hpp"

namespace shaykhraziev
{
  bool executeAnons(U2Storage& storage, std::ostream& output);
  bool executeDesc(U2Storage& storage, const std::string& line, std::ostream& output);
  bool executeRedesc(U2Storage& storage, const std::string& line);
  bool executeMeets(U2Storage& storage, const std::string& line, std::ostream& output);
  bool executeLess(U2Storage& storage, const std::string& line, std::ostream& output);
  bool executeGreater(U2Storage& storage, const std::string& line, std::ostream& output);
  bool executeCommons(U2Storage& storage, const std::string& line, std::ostream& output);
  bool executeDeanon(U2Storage& storage, const std::string& line);
  bool executeOutPersons(U2Storage& storage, const std::string& line, std::ostream& output);
  bool executeCommand(U2Storage& storage, const std::string& line, std::ostream& output);
  void runCommandLoop(U2Storage& storage, std::istream& input, std::ostream& output);
}

#endif
