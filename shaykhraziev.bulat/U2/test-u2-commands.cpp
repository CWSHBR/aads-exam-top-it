#include <boost/test/unit_test.hpp>
#include <cstdio>
#include <fstream>
#include "commands.hpp"
#include "../common/parse.hpp"
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

  void initCommandStorage(shaykhraziev::U2Storage& storage)
  {
    shaykhraziev::initU2Storage(storage);
    const bool known = true;
    shaykhraziev::insert(storage.knownIds, static_cast< size_t >(31), known);
    shaykhraziev::insert(storage.knownIds, static_cast< size_t >(32), known);
    shaykhraziev::insert(storage.knownIds, static_cast< size_t >(33), known);
    const shaykhraziev::Person person = { 31, "The Agent" };
    shaykhraziev::addPerson(storage.persons, storage.personsById, person);
  }

  void addMeeting(shaykhraziev::U2Storage& storage, size_t first, size_t second, size_t duration)
  {
    const shaykhraziev::Meeting meeting = { first, second, duration };
    shaykhraziev::pushBack(storage.meetings, meeting);
  }
}

BOOST_AUTO_TEST_CASE(anons_empty_output_when_no_anons)
{
  shaykhraziev::U2Storage storage;
  shaykhraziev::initU2Storage(storage);
  const bool known = true;
  shaykhraziev::insert(storage.knownIds, static_cast< size_t >(31), known);
  shaykhraziev::addPerson(storage.persons,
      storage.personsById,
      shaykhraziev::Person{ 31, "The Agent" });
  const char* filename = "out/u2-anons-empty.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeAnons(storage, output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "");

  shaykhraziev::clearU2Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(anons_sorted_output)
{
  shaykhraziev::U2Storage storage;
  initCommandStorage(storage);
  const char* filename = "out/u2-anons.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeAnons(storage, output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "32\n33\n");

  shaykhraziev::clearU2Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(desc_described_person)
{
  shaykhraziev::U2Storage storage;
  initCommandStorage(storage);
  const char* filename = "out/u2-desc-described.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeDesc(storage, "desc 31", output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "The Agent\n");

  shaykhraziev::clearU2Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(desc_anon_person)
{
  shaykhraziev::U2Storage storage;
  initCommandStorage(storage);
  const char* filename = "out/u2-desc-anon.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeDesc(storage, "desc 32", output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "<ANON>\n");

  shaykhraziev::clearU2Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(desc_unknown_person_is_invalid)
{
  shaykhraziev::U2Storage storage;
  initCommandStorage(storage);
  const char* filename = "out/u2-desc-unknown.txt";
  std::ofstream output(filename);

  BOOST_TEST(!shaykhraziev::executeDesc(storage, "desc 100", output));

  shaykhraziev::clearU2Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(redesc_updates_described_person)
{
  shaykhraziev::U2Storage storage;
  initCommandStorage(storage);

  BOOST_TEST(shaykhraziev::executeRedesc(storage, "redesc 31 \"Mr. Bond\""));
  BOOST_TEST(shaykhraziev::findPersonById(storage.personsById, 31)->info == "Mr. Bond");

  shaykhraziev::clearU2Storage(storage);
}

BOOST_AUTO_TEST_CASE(redesc_describes_anon_person)
{
  shaykhraziev::U2Storage storage;
  initCommandStorage(storage);

  BOOST_TEST(shaykhraziev::executeRedesc(storage, "redesc 32 \"Known\""));
  BOOST_TEST(shaykhraziev::findPersonById(storage.personsById, 32)->info == "Known");

  shaykhraziev::clearU2Storage(storage);
}

BOOST_AUTO_TEST_CASE(redesc_bad_quotes_is_invalid)
{
  shaykhraziev::U2Storage storage;
  initCommandStorage(storage);

  BOOST_TEST(!shaykhraziev::executeRedesc(storage, "redesc 32 Known"));

  shaykhraziev::clearU2Storage(storage);
}

BOOST_AUTO_TEST_CASE(meets_unknown_id_is_invalid)
{
  shaykhraziev::U2Storage storage;
  initCommandStorage(storage);
  const char* filename = "out/u2-meets-unknown.txt";
  std::ofstream output(filename);

  BOOST_TEST(!shaykhraziev::executeMeets(storage, "meets 100", output));

  shaykhraziev::clearU2Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(meets_without_values_outputs_nothing)
{
  shaykhraziev::U2Storage storage;
  initCommandStorage(storage);
  const char* filename = "out/u2-meets-empty.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeMeets(storage, "meets 31", output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "");

  shaykhraziev::clearU2Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(meets_outputs_sorted_values)
{
  shaykhraziev::U2Storage storage;
  initCommandStorage(storage);
  addMeeting(storage, 33, 31, 99);
  addMeeting(storage, 33, 32, 11);
  addMeeting(storage, 31, 33, 10);
  const char* filename = "out/u2-meets-sorted.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeMeets(storage, "meets 33", output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "31 10\n31 99\n32 11\n");

  shaykhraziev::clearU2Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(less_filters_meetings)
{
  shaykhraziev::U2Storage storage;
  initCommandStorage(storage);
  addMeeting(storage, 33, 31, 99);
  addMeeting(storage, 33, 32, 11);
  addMeeting(storage, 31, 33, 10);
  const char* filename = "out/u2-less.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeLess(storage, "less 50 33", output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "31 10\n32 11\n");

  shaykhraziev::clearU2Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(greater_filters_meetings)
{
  shaykhraziev::U2Storage storage;
  initCommandStorage(storage);
  addMeeting(storage, 33, 31, 99);
  addMeeting(storage, 33, 32, 11);
  addMeeting(storage, 31, 33, 10);
  const char* filename = "out/u2-greater.txt";
  std::ofstream output(filename);

  BOOST_TEST(shaykhraziev::executeGreater(storage, "greater 50 33", output));
  output.close();
  BOOST_TEST(readTextFile(filename) == "31 99\n");

  shaykhraziev::clearU2Storage(storage);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(meeting_query_rejects_bad_time)
{
  shaykhraziev::U2Storage storage;
  initCommandStorage(storage);
  const char* filename = "out/u2-bad-time.txt";
  std::ofstream output(filename);

  BOOST_TEST(!shaykhraziev::executeLess(storage, "less bad 33", output));
  BOOST_TEST(!shaykhraziev::executeGreater(storage, "greater 10 missing", output));

  shaykhraziev::clearU2Storage(storage);
  std::remove(filename);
}
