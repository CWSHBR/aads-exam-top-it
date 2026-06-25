#include <boost/test/unit_test.hpp>
#include <cstdio>
#include <fstream>
#include "commands.hpp"
#include "range-commands.hpp"
#include "../common/date.hpp"
#include "../common/person-io.hpp"

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

  void addKnown(shaykhraziev::U3Storage& storage, size_t id)
  {
    const bool known = true;
    shaykhraziev::insert(storage.knownIds, id, known);
  }

  void addMeeting(shaykhraziev::U3Storage& storage,
      shaykhraziev::Date date,
      size_t first,
      size_t second,
      size_t duration)
  {
    const shaykhraziev::DatedMeeting meeting = { date, first, second, duration };
    shaykhraziev::pushBack(storage.meetings, meeting);
    shaykhraziev::insertOrderedUniqueDate(storage.dates, date);
  }

  void initCommandStorage(shaykhraziev::U3Storage& storage)
  {
    shaykhraziev::initU3Storage(storage);
    addKnown(storage, 31);
    addKnown(storage, 32);
    addKnown(storage, 33);
    addKnown(storage, 41);
    shaykhraziev::addPerson(storage.persons,
        storage.personsById,
        shaykhraziev::Person{ 31, "Mr. Bond" });
    addMeeting(storage, shaykhraziev::Date{ 1, 1, 2024 }, 31, 33, 9);
    addMeeting(storage, shaykhraziev::Date{ 10, 1, 2024 }, 41, 33, 10);
    addMeeting(storage, shaykhraziev::Date{ 20, 1, 2024 }, 41, 32, 20);
    addMeeting(storage, shaykhraziev::Date{ 20, 1, 2024 }, 31, 32, 99);
    shaykhraziev::pushInitialRange(storage);
  }
}

BOOST_AUTO_TEST_CASE(u3_anons_and_desc)
{
  shaykhraziev::U3Storage storage;
  initCommandStorage(storage);
  const char* filename = "out/u3-anons-desc.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeAnons(storage, output));
  BOOST_TEST(shaykhraziev::executeDesc(storage, "desc 31", output));
  BOOST_TEST(shaykhraziev::executeDesc(storage, "desc 33", output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "32\n33\n41\nMr. Bond\n<ANON>\n");

  shaykhraziev::clearU3Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(u3_redesc_and_desc_update_anon)
{
  shaykhraziev::U3Storage storage;
  initCommandStorage(storage);
  const char* filename = "out/u3-redesc.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeRedesc(storage, "redesc 33 \"Agent 007\""));
  BOOST_TEST(shaykhraziev::executeDesc(storage, "desc 33", output));
  BOOST_TEST(shaykhraziev::executeDesc(storage, "desc 32 \"Known\"", output));
  BOOST_TEST(shaykhraziev::executeDesc(storage, "desc 32", output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "Agent 007\nKnown\n");

  shaykhraziev::clearU3Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(u3_meets_respects_range)
{
  shaykhraziev::U3Storage storage;
  initCommandStorage(storage);
  const char* filename = "out/u3-meets-range.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeAfter(storage, "after 10 1 2024"));
  BOOST_TEST(shaykhraziev::executeMeets(storage, "meets 31", output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "32 99\n");

  shaykhraziev::clearU3Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(u3_meet_after_acceptance_range)
{
  shaykhraziev::U3Storage storage;
  shaykhraziev::initU3Storage(storage);
  addKnown(storage, 31);
  addKnown(storage, 32);
  addKnown(storage, 33);
  addKnown(storage, 41);
  addMeeting(storage, shaykhraziev::Date{ 5, 5, 1005 }, 100, 33, 5);
  addMeeting(storage, shaykhraziev::Date{ 9, 99, 1700 }, 33, 31, 10);
  addMeeting(storage, shaykhraziev::Date{ 11, 33, 1945 }, 33, 32, 11);
  addMeeting(storage, shaykhraziev::Date{ 12, 33, 1946 }, 33, 41, 10);
  shaykhraziev::pushInitialRange(storage);
  const char* filename = "out/u3-meet-after-acceptance.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeAfter(storage, "after 10 10 1010"));
  BOOST_TEST(shaykhraziev::executeMeets(storage, "meet 33", output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "31 10\n32 11\n41 10\n");

  shaykhraziev::clearU3Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(u3_less_greater_respect_range)
{
  shaykhraziev::U3Storage storage;
  initCommandStorage(storage);
  const char* filename = "out/u3-duration-range.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeBefore(storage, "before 10 1 2024"));
  BOOST_TEST(shaykhraziev::executeLess(storage, "less 10 31", output));
  BOOST_TEST(shaykhraziev::executeGreater(storage, "greater 9 41", output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "33\n33\n");

  shaykhraziev::clearU3Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(u3_commons_respects_range)
{
  shaykhraziev::U3Storage storage;
  initCommandStorage(storage);
  const char* filename = "out/u3-commons-range.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeCommons(storage, "commons 31 41", output));
  BOOST_TEST(shaykhraziev::executeBefore(storage, "before 10 1 2024"));
  BOOST_TEST(shaykhraziev::executeCommons(storage, "commons 31 41", output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "32\n33\n33\n");

  shaykhraziev::clearU3Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(u3_empty_range_has_no_meeting_results)
{
  shaykhraziev::U3Storage storage;
  initCommandStorage(storage);
  const char* filename = "out/u3-empty-range-queries.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeAfter(storage, "after 1 1 2030"));
  BOOST_TEST(shaykhraziev::executeMeets(storage, "meet 31", output));
  BOOST_TEST(shaykhraziev::executeLess(storage, "less 100 31", output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "\n\n");

  shaykhraziev::clearU3Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(u3_deanon_updates_all_dated_meetings)
{
  shaykhraziev::U3Storage storage;
  initCommandStorage(storage);
  const char* filename = "out/u3-deanon.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeDeanon(storage, "deanon 33 31"));
  BOOST_TEST(!shaykhraziev::contains(storage.knownIds, static_cast< size_t >(33)));
  BOOST_TEST(shaykhraziev::executeMeets(storage, "meet 31", output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "32 99\n41 10\n");

  shaykhraziev::clearU3Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(u3_out_persons_writes_file_and_stdout)
{
  shaykhraziev::U3Storage storage;
  initCommandStorage(storage);
  const char* filename = "out/u3-persons-output.txt";
  const char* stdoutName = "out/u3-persons-stdout.txt";
  std::ofstream output(stdoutName);

  BOOST_TEST(shaykhraziev::executeDesc(storage, "desc 33 \"Agent 007\"", output));
  BOOST_TEST(shaykhraziev::executeOutPersons(storage, std::string("out-persons ") + filename, output));
  output.close();
  BOOST_TEST(readTextFile(stdoutName) == "31 Mr. Bond\n33 Agent 007\n");
  BOOST_TEST(readTextFile(filename) == "31 Mr. Bond\n33 Agent 007\n");

  shaykhraziev::clearU3Storage(storage);
  std::remove(filename);
  std::remove(stdoutName);
}

BOOST_AUTO_TEST_CASE(u3_invalid_command_formats)
{
  shaykhraziev::U3Storage storage;
  initCommandStorage(storage);
  const char* filename = "out/u3-invalid.txt";
  std::ofstream output(filename);

  BOOST_TEST(!shaykhraziev::executeCommand(storage, "anons extra", output));
  BOOST_TEST(!shaykhraziev::executeCommand(storage, "desc 999", output));
  BOOST_TEST(!shaykhraziev::executeCommand(storage, "less bad 31", output));

  shaykhraziev::clearU3Storage(storage);
  std::remove(filename);
}
