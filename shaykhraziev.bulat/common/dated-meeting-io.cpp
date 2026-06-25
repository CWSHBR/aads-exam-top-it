#include "../common/dated-meeting-io.hpp"
#include <istream>
#include "../common/date.hpp"
#include "../common/parse.hpp"

bool shaykhraziev::parseDatedMeetingLine(const std::string& line, DatedMeeting& meeting)
{
  size_t position = 0;
  size_t day = 0;
  size_t month = 0;
  size_t year = 0;
  size_t first = 0;
  size_t second = 0;
  size_t duration = 0;
  if (!parseSizeTPrefix(line, day, position))
  {
    return false;
  }
  if (!parseSizeTPrefix(line, month, position))
  {
    return false;
  }
  if (!parseSizeTPrefix(line, year, position))
  {
    return false;
  }
  if (!parseSizeTPrefix(line, first, position))
  {
    return false;
  }
  if (!parseSizeTPrefix(line, second, position))
  {
    return false;
  }
  if (!parseSizeTPrefix(line, duration, position))
  {
    return false;
  }
  if (skipSpaces(line, position) != line.size())
  {
    return false;
  }
  meeting.date = Date{ day, month, year };
  meeting.first = first;
  meeting.second = second;
  meeting.duration = duration;
  return true;
}

bool shaykhraziev::readDatedMeetings(std::istream& input,
    List< DatedMeeting >& meetings,
    HashTable< size_t, bool >& knownIds,
    List< Date >& dates)
{
  std::string line;
  while (std::getline(input, line))
  {
    if (skipSpaces(line, 0) == line.size())
    {
      continue;
    }
    DatedMeeting meeting = { Date{ 0, 0, 0 }, 0, 0, 0 };
    if (!parseDatedMeetingLine(line, meeting))
    {
      return false;
    }
    insertOrderedUniqueDate(dates, meeting.date);
    if (meeting.first != meeting.second)
    {
      const bool known = true;
      insert(knownIds, meeting.first, known);
      insert(knownIds, meeting.second, known);
      pushBack(meetings, meeting);
    }
  }
  return true;
}
