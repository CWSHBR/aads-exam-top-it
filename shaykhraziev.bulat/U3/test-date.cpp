#define BOOST_TEST_MODULE U3
#include <boost/test/included/unit_test.hpp>
#include "../common/date.hpp"

BOOST_AUTO_TEST_CASE(compare_dates_by_year_month_day)
{
  const shaykhraziev::Date early = { 31, 12, 2023 };
  const shaykhraziev::Date middle = { 1, 1, 2024 };
  const shaykhraziev::Date late = { 2, 1, 2024 };

  BOOST_TEST(shaykhraziev::compareDates(early, middle) < 0);
  BOOST_TEST(shaykhraziev::compareDates(late, middle) > 0);
  BOOST_TEST(shaykhraziev::isDateLess(middle, late));
  BOOST_TEST(shaykhraziev::isDateLessOrEqual(middle, middle));
  BOOST_TEST(shaykhraziev::isDateGreaterOrEqual(late, middle));
}

BOOST_AUTO_TEST_CASE(date_equality)
{
  const shaykhraziev::Date first = { 10, 5, 2024 };
  const shaykhraziev::Date second = { 10, 5, 2024 };

  BOOST_TEST(shaykhraziev::isDateEqual(first, second));
  BOOST_TEST(!shaykhraziev::isDateLess(first, second));
}

BOOST_AUTO_TEST_CASE(ordered_unique_date_insert)
{
  shaykhraziev::List< shaykhraziev::Date > dates;
  shaykhraziev::initList(dates);

  BOOST_TEST(shaykhraziev::insertOrderedUniqueDate(dates, shaykhraziev::Date{ 10, 5, 2024 }));
  BOOST_TEST(shaykhraziev::insertOrderedUniqueDate(dates, shaykhraziev::Date{ 1, 1, 2024 }));
  BOOST_TEST(!shaykhraziev::insertOrderedUniqueDate(dates, shaykhraziev::Date{ 10, 5, 2024 }));
  BOOST_TEST(shaykhraziev::insertOrderedUniqueDate(dates, shaykhraziev::Date{ 31, 12, 2023 }));

  BOOST_TEST(dates.size == 3);
  BOOST_TEST(dates.head->value.day == 31);
  BOOST_TEST(dates.tail->value.day == 10);
  BOOST_TEST(dates.tail->value.month == 5);

  shaykhraziev::clearList(dates);
}

BOOST_AUTO_TEST_CASE(date_inside_range)
{
  const shaykhraziev::DateRange range = {
    shaykhraziev::Date{ 1, 1, 2024 },
    shaykhraziev::Date{ 31, 1, 2024 },
    false
  };
  const shaykhraziev::DateRange empty = {
    shaykhraziev::Date{ 1, 1, 2024 },
    shaykhraziev::Date{ 31, 1, 2024 },
    true
  };

  BOOST_TEST(shaykhraziev::isDateInsideRange(shaykhraziev::Date{ 1, 1, 2024 }, range));
  BOOST_TEST(shaykhraziev::isDateInsideRange(shaykhraziev::Date{ 10, 1, 2024 }, range));
  BOOST_TEST(shaykhraziev::isDateInsideRange(shaykhraziev::Date{ 31, 1, 2024 }, range));
  BOOST_TEST(!shaykhraziev::isDateInsideRange(shaykhraziev::Date{ 1, 2, 2024 }, range));
  BOOST_TEST(!shaykhraziev::isDateInsideRange(shaykhraziev::Date{ 10, 1, 2024 }, empty));
}
