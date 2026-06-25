#ifndef DATED_MEETING_IO_HPP
#define DATED_MEETING_IO_HPP

#include <iosfwd>
#include <string>
#include "../common/domain.hpp"
#include "../common/hash-table.hpp"
#include "../common/list.hpp"

namespace shaykhraziev
{
  bool parseDatedMeetingLine(const std::string& line, DatedMeeting& meeting);
  bool readDatedMeetings(std::istream& input,
      List< DatedMeeting >& meetings,
      HashTable< size_t, bool >& knownIds,
      List< Date >& dates);
}

#endif
