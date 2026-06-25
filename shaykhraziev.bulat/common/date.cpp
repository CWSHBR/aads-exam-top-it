#include "../common/date.hpp"
#include <ostream>
#include "../common/ordered-list.hpp"

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
}

int shaykhraziev::compareDates(const Date& left, const Date& right)
{
  if (left.year != right.year)
  {
    return (left.year < right.year) ? -1 : 1;
  }
  if (left.month != right.month)
  {
    return (left.month < right.month) ? -1 : 1;
  }
  if (left.day != right.day)
  {
    return (left.day < right.day) ? -1 : 1;
  }
  return 0;
}

bool shaykhraziev::isDateLess(const Date& left, const Date& right)
{
  return compareDates(left, right) < 0;
}

bool shaykhraziev::isDateEqual(const Date& left, const Date& right)
{
  return compareDates(left, right) == 0;
}

bool shaykhraziev::isDateLessOrEqual(const Date& left, const Date& right)
{
  return compareDates(left, right) <= 0;
}

bool shaykhraziev::isDateGreaterOrEqual(const Date& left, const Date& right)
{
  return compareDates(left, right) >= 0;
}

void shaykhraziev::printDate(std::ostream& output, const Date& date)
{
  output << date.day << ' ' << date.month << ' ' << date.year;
}

bool shaykhraziev::insertOrderedUniqueDate(List< Date >& dates, const Date& date)
{
  ListIterator< Date > iterator = begin(dates);
  while (!isEnd(iterator))
  {
    if (isDateEqual(get(iterator), date))
    {
      return false;
    }
    if (isDateLess(date, get(iterator)))
    {
      insertBefore(dates, iterator, date);
      return true;
    }
    iterator = next(iterator);
  }
  pushBack(dates, date);
  return true;
}

bool shaykhraziev::isDateInsideRange(const Date& date, const DateRange& range)
{
  return !range.empty && isDateGreaterOrEqual(date, range.from)
      && isDateLessOrEqual(date, range.to)
      && !isInputDateLess(date, range.from);
}
