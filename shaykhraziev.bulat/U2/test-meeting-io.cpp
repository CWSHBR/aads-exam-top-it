#define BOOST_TEST_MODULE U2
#include <boost/test/included/unit_test.hpp>
#include <cstdio>
#include <fstream>
#include "../common/meeting-io.hpp"
#include "../common/parse.hpp"

namespace
{
  void writeTextFile(const char* filename, const char* text)
  {
    std::ofstream output(filename);
    output << text;
  }

  void initMeetingStorage(shaykhraziev::List< shaykhraziev::Meeting >& meetings,
      shaykhraziev::HashTable< size_t, bool >& knownIds)
  {
    shaykhraziev::initList(meetings);
    shaykhraziev::initHashTable(knownIds, 17, shaykhraziev::hashSizeT, shaykhraziev::equalSizeT);
  }

  void clearMeetingStorage(shaykhraziev::List< shaykhraziev::Meeting >& meetings,
      shaykhraziev::HashTable< size_t, bool >& knownIds)
  {
    shaykhraziev::clearHashTable(knownIds);
    shaykhraziev::clearList(meetings);
  }
}

BOOST_AUTO_TEST_CASE(parse_correct_meeting_line)
{
  shaykhraziev::Meeting meeting = { 0, 0, 0 };

  BOOST_TEST(shaykhraziev::parseMeetingLine("33 41 10", meeting));
  BOOST_TEST(meeting.first == 33);
  BOOST_TEST(meeting.second == 41);
  BOOST_TEST(meeting.duration == 10);
}

BOOST_AUTO_TEST_CASE(reject_bad_meeting_line)
{
  shaykhraziev::Meeting meeting = { 0, 0, 0 };

  BOOST_TEST(!shaykhraziev::parseMeetingLine("33 41 bad", meeting));
  BOOST_TEST(!shaykhraziev::parseMeetingLine("33 41 10 extra", meeting));
}

BOOST_AUTO_TEST_CASE(read_meetings_skips_self_meeting)
{
  const char* filename = "out/u2-self-meeting.txt";
  writeTextFile(filename, "33 33 100\n");
  std::ifstream input(filename);
  shaykhraziev::List< shaykhraziev::Meeting > meetings;
  shaykhraziev::HashTable< size_t, bool > knownIds;
  initMeetingStorage(meetings, knownIds);

  BOOST_TEST(shaykhraziev::readMeetings(input, meetings, knownIds));
  BOOST_TEST(meetings.size == 0);
  BOOST_TEST(!shaykhraziev::contains(knownIds, static_cast< size_t >(33)));

  clearMeetingStorage(meetings, knownIds);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(read_meetings_adds_unknown_ids)
{
  const char* filename = "out/u2-known-meeting.txt";
  writeTextFile(filename, "33 41 10\n32 33 99\n");
  std::ifstream input(filename);
  shaykhraziev::List< shaykhraziev::Meeting > meetings;
  shaykhraziev::HashTable< size_t, bool > knownIds;
  initMeetingStorage(meetings, knownIds);

  BOOST_TEST(shaykhraziev::readMeetings(input, meetings, knownIds));
  BOOST_TEST(meetings.size == 2);
  BOOST_TEST(shaykhraziev::contains(knownIds, static_cast< size_t >(33)));
  BOOST_TEST(shaykhraziev::contains(knownIds, static_cast< size_t >(41)));
  BOOST_TEST(shaykhraziev::contains(knownIds, static_cast< size_t >(32)));

  clearMeetingStorage(meetings, knownIds);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(read_meetings_reports_parse_error)
{
  const char* filename = "out/u2-bad-meeting.txt";
  writeTextFile(filename, "33 41 10\nbroken\n");
  std::ifstream input(filename);
  shaykhraziev::List< shaykhraziev::Meeting > meetings;
  shaykhraziev::HashTable< size_t, bool > knownIds;
  initMeetingStorage(meetings, knownIds);

  BOOST_TEST(!shaykhraziev::readMeetings(input, meetings, knownIds));

  clearMeetingStorage(meetings, knownIds);
  std::remove(filename);
}
