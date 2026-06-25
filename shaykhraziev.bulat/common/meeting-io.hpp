#ifndef MEETING_IO_HPP
#define MEETING_IO_HPP

#include <iosfwd>
#include "../common/domain.hpp"
#include "../common/hash-table.hpp"
#include "../common/list.hpp"

namespace shaykhraziev
{
  bool parseMeetingLine(const std::string& line, Meeting& meeting);
  bool readMeetings(std::istream& input,
      List< Meeting >& meetings,
      HashTable< size_t, bool >& knownIds);
}

#endif
