#include "range-commands.hpp"
#include <ostream>
#include "../common/date.hpp"
#include "../common/parse.hpp"

namespace
{
  bool isInputDateLess(const shaykhraziev::Date& left, const shaykhraziev::Date& right)
  {
    if (left.day != right.day)
    {
      return left.day < right.day;
    }
    if (left.month != right.month)
    {
      return left.month < right.month;
    }
    return left.year < right.year;
  }

  bool parseCommandPrefix(const std::string& line, const char* command, size_t& position)
  {
    if (!shaykhraziev::startsWith(line, command))
    {
      return false;
    }
    position = 0;
    while (command[position] != '\0')
    {
      ++position;
    }
    if ((position < line.size()) && (line[position] != ' ') && (line[position] != '\t'))
    {
      return false;
    }
    return true;
  }

  bool parseExactCommand(const std::string& line, const char* command)
  {
    size_t position = 0;
    if (!parseCommandPrefix(line, command, position))
    {
      return false;
    }
    return shaykhraziev::skipSpaces(line, position) == line.size();
  }

  bool parseDateCommand(const std::string& line, const char* command, shaykhraziev::Date& date)
  {
    size_t position = 0;
    size_t day = 0;
    size_t month = 0;
    size_t year = 0;
    if (!parseCommandPrefix(line, command, position))
    {
      return false;
    }
    if (!shaykhraziev::parseSizeTPrefix(line, day, position))
    {
      return false;
    }
    if (!shaykhraziev::parseSizeTPrefix(line, month, position))
    {
      return false;
    }
    if (!shaykhraziev::parseSizeTPrefix(line, year, position))
    {
      return false;
    }
    if (shaykhraziev::skipSpaces(line, position) != line.size())
    {
      return false;
    }
    date = shaykhraziev::Date{ day, month, year };
    return true;
  }
}

shaykhraziev::DateRange* shaykhraziev::getCurrentRange(U3Storage& storage)
{
  if (storage.ranges.tail == nullptr)
  {
    return nullptr;
  }
  return &storage.ranges.tail->value;
}

void shaykhraziev::printCurrentRange(U3Storage& storage, std::ostream& output)
{
  DateRange* range = getCurrentRange(storage);
  if ((range == nullptr) || range->empty)
  {
    output << "<EMPTY>\n";
    return;
  }
  printDate(output, range->from);
  output << " : ";
  printDate(output, range->to);
  output << '\n';
}

bool shaykhraziev::executeRange(U3Storage& storage,
    const std::string& line,
    std::ostream& output)
{
  if (!parseExactCommand(line, "range"))
  {
    return false;
  }
  printCurrentRange(storage, output);
  return true;
}

bool shaykhraziev::executeAfter(U3Storage& storage, const std::string& line)
{
  Date requested = { 0, 0, 0 };
  DateRange* current = getCurrentRange(storage);
  if (!parseDateCommand(line, "after", requested) || (current == nullptr) || current->empty)
  {
    return false;
  }
  if (isInputDateLess(current->to, requested))
  {
    const DateRange range = { Date{ 0, 0, 0 }, Date{ 0, 0, 0 }, true };
    pushBack(storage.ranges, range);
    return true;
  }
  ListIterator< Date > iterator = begin(storage.dates);
  while (!isEnd(iterator))
  {
    const Date& date = get(iterator);
    if (isDateInsideRange(date, *current) && isDateGreaterOrEqual(date, requested))
    {
      const DateRange range = { date, current->to, false };
      pushBack(storage.ranges, range);
      return true;
    }
    iterator = next(iterator);
  }
  const DateRange range = { Date{ 0, 0, 0 }, Date{ 0, 0, 0 }, true };
  pushBack(storage.ranges, range);
  return true;
}

bool shaykhraziev::executeBefore(U3Storage& storage, const std::string& line)
{
  Date requested = { 0, 0, 0 };
  DateRange* current = getCurrentRange(storage);
  if (!parseDateCommand(line, "before", requested) || (current == nullptr) || current->empty)
  {
    return false;
  }
  bool found = false;
  Date last = { 0, 0, 0 };
  ListIterator< Date > iterator = begin(storage.dates);
  while (!isEnd(iterator))
  {
    const Date& date = get(iterator);
    if (isDateInsideRange(date, *current) && isDateLessOrEqual(date, requested))
    {
      last = date;
      found = true;
    }
    iterator = next(iterator);
  }
  if (!found)
  {
    const DateRange range = { Date{ 0, 0, 0 }, Date{ 0, 0, 0 }, true };
    pushBack(storage.ranges, range);
  }
  else
  {
    const DateRange range = { current->from, last, false };
    pushBack(storage.ranges, range);
  }
  return true;
}

bool shaykhraziev::executePopRange(U3Storage& storage, const std::string& line)
{
  if (!parseExactCommand(line, "pop-range") || (storage.ranges.size <= 1))
  {
    return false;
  }
  popBack(storage.ranges);
  return true;
}
