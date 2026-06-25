#include "../common/meeting-io.hpp"
#include <istream>
#include "../common/parse.hpp"

bool shaykhraziev::parseMeetingLine(const std::string& line, Meeting& meeting)
{
  size_t position = 0;
  size_t first = 0;
  size_t second = 0;
  size_t duration = 0;
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
  meeting.first = first;
  meeting.second = second;
  meeting.duration = duration;
  return true;
}

bool shaykhraziev::readMeetings(std::istream& input,
    List< Meeting >& meetings,
    HashTable< size_t, bool >& knownIds)
{
  std::string line;
  while (std::getline(input, line))
  {
    if (skipSpaces(line, 0) == line.size())
    {
      continue;
    }
    Meeting meeting = { 0, 0, 0 };
    if (!parseMeetingLine(line, meeting))
    {
      return false;
    }
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
