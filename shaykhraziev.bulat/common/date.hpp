#ifndef DATE_HPP
#define DATE_HPP

#include <iosfwd>
#include "../common/domain.hpp"
#include "../common/list.hpp"

namespace shaykhraziev
{
  int compareDates(const Date& left, const Date& right);
  bool isDateLess(const Date& left, const Date& right);
  bool isDateEqual(const Date& left, const Date& right);
  bool isDateLessOrEqual(const Date& left, const Date& right);
  bool isDateGreaterOrEqual(const Date& left, const Date& right);
  void printDate(std::ostream& output, const Date& date);
  bool insertOrderedUniqueDate(List< Date >& dates, const Date& date);
  bool isDateInsideRange(const Date& date, const DateRange& range);
}

#endif
