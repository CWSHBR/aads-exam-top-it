#include <boost/test/unit_test.hpp>
#include <cstdio>
#include <fstream>
#include "range-commands.hpp"
#include "../common/date.hpp"

namespace
{
  std::string readTextFile(const char* filename)
  {
    std::ifstream input(filename);
    std::string result;
    char symbol = '\0';
    while (input.get(symbol))
    {
      result += symbol;
    }
    return result;
  }

  void addDate(shaykhraziev::U3Storage& storage, size_t day, size_t month, size_t year)
  {
    shaykhraziev::insertOrderedUniqueDate(storage.dates, shaykhraziev::Date{ day, month, year });
  }

  void initStorageWithDates(shaykhraziev::U3Storage& storage)
  {
    shaykhraziev::initU3Storage(storage);
    addDate(storage, 1, 1, 2024);
    addDate(storage, 10, 1, 2024);
    addDate(storage, 20, 1, 2024);
    shaykhraziev::pushInitialRange(storage);
  }
}

BOOST_AUTO_TEST_CASE(range_prints_current_range)
{
  shaykhraziev::U3Storage storage;
  initStorageWithDates(storage);
  const char* filename = "out/u3-range.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeRange(storage, "range", output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "1 1 2024 : 20 1 2024\n");

  shaykhraziev::clearU3Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(range_prints_empty)
{
  shaykhraziev::U3Storage storage;
  shaykhraziev::initU3Storage(storage);
  shaykhraziev::pushInitialRange(storage);
  const char* filename = "out/u3-range-empty.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeRange(storage, "range", output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "<EMPTY>\n");

  shaykhraziev::clearU3Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(after_before_narrow_range)
{
  shaykhraziev::U3Storage storage;
  initStorageWithDates(storage);

  BOOST_TEST(shaykhraziev::executeAfter(storage, "after 5 1 2024"));
  BOOST_TEST(shaykhraziev::getCurrentRange(storage)->from.day == 10);
  BOOST_TEST(shaykhraziev::getCurrentRange(storage)->to.day == 20);
  BOOST_TEST(shaykhraziev::executeBefore(storage, "before 15 1 2024"));
  BOOST_TEST(shaykhraziev::getCurrentRange(storage)->from.day == 10);
  BOOST_TEST(shaykhraziev::getCurrentRange(storage)->to.day == 10);

  shaykhraziev::clearU3Storage(storage);
}

BOOST_AUTO_TEST_CASE(after_before_can_make_empty_range)
{
  shaykhraziev::U3Storage storage;
  initStorageWithDates(storage);

  BOOST_TEST(shaykhraziev::executeAfter(storage, "after 1 2 2024"));
  BOOST_TEST(shaykhraziev::getCurrentRange(storage)->empty);
  BOOST_TEST(!shaykhraziev::executeBefore(storage, "before 1 1 2024"));

  shaykhraziev::clearU3Storage(storage);
  initStorageWithDates(storage);
  BOOST_TEST(shaykhraziev::executeBefore(storage, "before 31 12 2023"));
  BOOST_TEST(shaykhraziev::getCurrentRange(storage)->empty);
  BOOST_TEST(!shaykhraziev::executeAfter(storage, "after 1 1 2024"));

  shaykhraziev::clearU3Storage(storage);
}

BOOST_AUTO_TEST_CASE(pop_range_rolls_back)
{
  shaykhraziev::U3Storage storage;
  initStorageWithDates(storage);

  BOOST_TEST(!shaykhraziev::executePopRange(storage, "pop-range"));
  BOOST_TEST(shaykhraziev::executeAfter(storage, "after 5 1 2024"));
  BOOST_TEST(storage.ranges.size == 2);
  BOOST_TEST(shaykhraziev::executePopRange(storage, "pop-range"));
  BOOST_TEST(storage.ranges.size == 1);
  BOOST_TEST(shaykhraziev::getCurrentRange(storage)->from.day == 1);

  shaykhraziev::clearU3Storage(storage);
}

BOOST_AUTO_TEST_CASE(range_accepts_non_calendar_date_values)
{
  shaykhraziev::U3Storage storage;
  shaykhraziev::initU3Storage(storage);
  addDate(storage, 12, 33, 1946);
  addDate(storage, 9, 99, 1700);
  addDate(storage, 5, 5, 1005);
  shaykhraziev::pushInitialRange(storage);
  const char* filename = "out/u3-non-calendar-range.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeRange(storage, "range", output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "5 5 1005 : 12 33 1946\n");

  shaykhraziev::clearU3Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(after_empty_range_rejects_next_after)
{
  shaykhraziev::U3Storage storage;
  shaykhraziev::initU3Storage(storage);
  addDate(storage, 5, 5, 1005);
  addDate(storage, 9, 99, 1700);
  shaykhraziev::pushInitialRange(storage);
  const char* filename = "out/u3-after-empty-range.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeAfter(storage, "after 65 832 1889"));
  BOOST_TEST(shaykhraziev::executeRange(storage, "range", output));
  BOOST_TEST(!shaykhraziev::executeAfter(storage, "after 10 10 1010"));
  output.close();
  BOOST_TEST(readTextFile(filename) == "<EMPTY>\n");

  shaykhraziev::clearU3Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(before_empty_and_pop_range_restore_initial)
{
  shaykhraziev::U3Storage storage;
  shaykhraziev::initU3Storage(storage);
  addDate(storage, 5, 5, 1005);
  addDate(storage, 12, 33, 1946);
  shaykhraziev::pushInitialRange(storage);
  const char* filename = "out/u3-before-pop-range.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeBefore(storage, "before 5 5 1004"));
  BOOST_TEST(shaykhraziev::executeRange(storage, "range", output));
  BOOST_TEST(!shaykhraziev::executeBefore(storage, "before 1 1 1000"));
  BOOST_TEST(shaykhraziev::executePopRange(storage, "pop-range"));
  BOOST_TEST(shaykhraziev::executeRange(storage, "range", output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "<EMPTY>\n5 5 1005 : 12 33 1946\n");

  shaykhraziev::clearU3Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(after_before_pop_keeps_previous_non_calendar_range)
{
  shaykhraziev::U3Storage storage;
  shaykhraziev::initU3Storage(storage);
  addDate(storage, 9, 99, 1700);
  addDate(storage, 11, 33, 1945);
  addDate(storage, 12, 33, 1946);
  shaykhraziev::pushInitialRange(storage);
  const char* filename = "out/u3-after-before-pop-range.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeAfter(storage, "after 11 10 1010"));
  BOOST_TEST(shaykhraziev::executeBefore(storage, "before 12 33 1945"));
  BOOST_TEST(shaykhraziev::executeRange(storage, "range", output));
  BOOST_TEST(shaykhraziev::executePopRange(storage, "pop-range"));
  BOOST_TEST(shaykhraziev::executeRange(storage, "range", output));
  output.close();
  BOOST_TEST(readTextFile(filename) ==
      "9 99 1700 : 11 33 1945\n"
      "9 99 1700 : 12 33 1946\n");

  shaykhraziev::clearU3Storage(storage);
  std::remove(filename);
}
