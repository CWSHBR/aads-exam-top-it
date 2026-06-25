#include <boost/test/unit_test.hpp>
#include <cstdio>
#include <fstream>
#include "program.hpp"
#include "../common/dated-meeting-io.hpp"
#include "../common/parse.hpp"

namespace
{
  void writeTextFile(const char* filename, const char* text)
  {
    std::ofstream output(filename);
    output << text;
  }

}

BOOST_AUTO_TEST_CASE(parse_dated_meeting_line)
{
  shaykhraziev::DatedMeeting meeting = { shaykhraziev::Date{ 0, 0, 0 }, 0, 0, 0 };

  BOOST_TEST(shaykhraziev::parseDatedMeetingLine("10 5 2024 31 33 99", meeting));
  BOOST_TEST(meeting.date.day == 10);
  BOOST_TEST(meeting.date.month == 5);
  BOOST_TEST(meeting.date.year == 2024);
  BOOST_TEST(meeting.first == 31);
  BOOST_TEST(meeting.second == 33);
  BOOST_TEST(meeting.duration == 99);
  BOOST_TEST(shaykhraziev::parseDatedMeetingLine("9 99 1700 31 33 10", meeting));
  BOOST_TEST(meeting.date.day == 9);
  BOOST_TEST(meeting.date.month == 99);
  BOOST_TEST(meeting.date.year == 1700);
  BOOST_TEST(!shaykhraziev::parseDatedMeetingLine("10 5 2024 31 33", meeting));
  BOOST_TEST(!shaykhraziev::parseDatedMeetingLine("10 5 2024 31 33 99 tail", meeting));
}

BOOST_AUTO_TEST_CASE(read_dated_meetings_updates_known_ids_and_dates)
{
  const char* filename = "out/u3-dated-meetings.txt";
  writeTextFile(filename, "10 5 2024 31 33 99\n1 1 2024 41 32 10\n10 5 2024 33 33 1\n");
  std::ifstream input(filename);
  shaykhraziev::List< shaykhraziev::DatedMeeting > meetings;
  shaykhraziev::List< shaykhraziev::Date > dates;
  shaykhraziev::HashTable< size_t, bool > knownIds;
  shaykhraziev::initList(meetings);
  shaykhraziev::initList(dates);
  shaykhraziev::initHashTable(knownIds, 17, shaykhraziev::hashSizeT, shaykhraziev::equalSizeT);

  BOOST_TEST(shaykhraziev::readDatedMeetings(input, meetings, knownIds, dates));
  BOOST_TEST(meetings.size == 2);
  BOOST_TEST(dates.size == 2);
  BOOST_TEST(dates.head->value.day == 1);
  BOOST_TEST(dates.tail->value.day == 10);
  BOOST_TEST(shaykhraziev::contains(knownIds, static_cast< size_t >(31)));
  BOOST_TEST(!shaykhraziev::contains(knownIds, static_cast< size_t >(99)));

  shaykhraziev::clearHashTable(knownIds);
  shaykhraziev::clearList(dates);
  shaykhraziev::clearList(meetings);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(read_dated_meetings_keeps_self_meeting_dates)
{
  const char* filename = "out/u3-self-dated-meeting.txt";
  writeTextFile(filename, "9 99 1700 33 33 1\n");
  std::ifstream input(filename);
  shaykhraziev::List< shaykhraziev::DatedMeeting > meetings;
  shaykhraziev::List< shaykhraziev::Date > dates;
  shaykhraziev::HashTable< size_t, bool > knownIds;
  shaykhraziev::initList(meetings);
  shaykhraziev::initList(dates);
  shaykhraziev::initHashTable(knownIds, 17, shaykhraziev::hashSizeT, shaykhraziev::equalSizeT);

  BOOST_TEST(shaykhraziev::readDatedMeetings(input, meetings, knownIds, dates));
  BOOST_TEST(meetings.size == 0);
  BOOST_TEST(dates.size == 1);
  BOOST_TEST(dates.head->value.day == 9);
  BOOST_TEST(dates.head->value.month == 99);
  BOOST_TEST(dates.head->value.year == 1700);

  shaykhraziev::clearHashTable(knownIds);
  shaykhraziev::clearList(dates);
  shaykhraziev::clearList(meetings);
  std::remove(filename);
}

BOOST_AUTO_TEST_CASE(parse_u3_args_accepts_many_data_files)
{
  char arg0[] = "lab";
  char arg1[] = "data:first";
  char arg2[] = "in:persons";
  char arg3[] = "data:second";
  char* argv[] = { arg0, arg1, arg2, arg3 };
  shaykhraziev::U3Args args;
  shaykhraziev::initU3Args(args);

  BOOST_TEST(shaykhraziev::parseU3Args(4, argv, args));
  BOOST_TEST(args.hasInput);
  BOOST_TEST(args.inputName == "persons");
  BOOST_TEST(args.dataNames.size == 2);

  shaykhraziev::clearU3Args(args);
}

BOOST_AUTO_TEST_CASE(parse_u3_args_rejects_bad_and_repeated_input)
{
  char arg0[] = "lab";
  char arg1[] = "in:first";
  char arg2[] = "in:second";
  char* repeated[] = { arg0, arg1, arg2 };
  char bad[] = "out:file";
  char* unknown[] = { arg0, bad };
  shaykhraziev::U3Args args;
  shaykhraziev::initU3Args(args);

  BOOST_TEST(!shaykhraziev::parseU3Args(3, repeated, args));
  BOOST_TEST(!shaykhraziev::parseU3Args(2, unknown, args));

  shaykhraziev::clearU3Args(args);
}

BOOST_AUTO_TEST_CASE(load_u3_data_from_several_files)
{
  const char* personsName = "out/u3-persons.txt";
  const char* firstData = "out/u3-data-first.txt";
  const char* secondData = "out/u3-data-second.txt";
  writeTextFile(personsName, "31 Mr. Bond\n100 Sneaky person\n");
  writeTextFile(firstData, "10 5 2024 31 33 99\n");
  writeTextFile(secondData, "1 1 2024 41 32 10\n");
  char arg0[] = "lab";
  char arg1[] = "in:out/u3-persons.txt";
  char arg2[] = "data:out/u3-data-first.txt";
  char arg3[] = "data:out/u3-data-second.txt";
  char* argv[] = { arg0, arg1, arg2, arg3 };
  shaykhraziev::U3Args args;
  shaykhraziev::U3Storage storage;
  shaykhraziev::initU3Args(args);
  shaykhraziev::initU3Storage(storage);

  BOOST_TEST(shaykhraziev::parseU3Args(4, argv, args));
  BOOST_TEST(shaykhraziev::loadU3Data(args, storage) == 0);
  BOOST_TEST(storage.persons.size == 2);
  BOOST_TEST(storage.meetings.size == 2);
  BOOST_TEST(storage.dates.size == 2);
  BOOST_TEST(storage.ranges.size == 1);
  BOOST_TEST(!storage.ranges.tail->value.empty);
  BOOST_TEST(storage.ranges.tail->value.from.day == 1);
  BOOST_TEST(storage.ranges.tail->value.to.day == 10);

  shaykhraziev::clearU3Storage(storage);
  shaykhraziev::clearU3Args(args);
  std::remove(personsName);
  std::remove(firstData);
  std::remove(secondData);
}

BOOST_AUTO_TEST_CASE(load_u3_data_allows_no_data_files)
{
  char arg0[] = "lab";
  char* argv[] = { arg0 };
  shaykhraziev::U3Args args;
  shaykhraziev::U3Storage storage;
  shaykhraziev::initU3Args(args);
  shaykhraziev::initU3Storage(storage);

  BOOST_TEST(shaykhraziev::parseU3Args(1, argv, args));
  BOOST_TEST(shaykhraziev::loadU3Data(args, storage) == 0);
  BOOST_TEST(storage.ranges.size == 1);
  BOOST_TEST(storage.ranges.tail->value.empty);

  shaykhraziev::clearU3Storage(storage);
  shaykhraziev::clearU3Args(args);
}

BOOST_AUTO_TEST_CASE(load_u3_data_reports_open_and_format_errors)
{
  const char* badData = "out/u3-bad-data.txt";
  writeTextFile(badData, "10 5 2024 31 33 bad\n");
  char arg0[] = "lab";
  char missingArg[] = "data:no-such-file";
  char badArg[] = "data:out/u3-bad-data.txt";
  char* missing[] = { arg0, missingArg };
  char* bad[] = { arg0, badArg };
  shaykhraziev::U3Args args;
  shaykhraziev::U3Storage storage;
  shaykhraziev::initU3Args(args);
  shaykhraziev::initU3Storage(storage);

  BOOST_TEST(shaykhraziev::parseU3Args(2, missing, args));
  BOOST_TEST(shaykhraziev::loadU3Data(args, storage) == 2);
  shaykhraziev::clearU3Storage(storage);
  shaykhraziev::clearU3Args(args);
  shaykhraziev::initU3Args(args);
  shaykhraziev::initU3Storage(storage);
  BOOST_TEST(shaykhraziev::parseU3Args(2, bad, args));
  BOOST_TEST(shaykhraziev::loadU3Data(args, storage) == 3);

  shaykhraziev::clearU3Storage(storage);
  shaykhraziev::clearU3Args(args);
  std::remove(badData);
}
