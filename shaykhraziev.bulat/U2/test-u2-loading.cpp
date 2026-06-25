#include <boost/test/unit_test.hpp>
#include <cstdio>
#include <fstream>
#include "program.hpp"

namespace
{
  void writeTextFile(const char* filename, const char* text)
  {
    std::ofstream output(filename);
    output << text;
  }
}

BOOST_AUTO_TEST_CASE(parse_u2_data_only_args)
{
  char program[] = "lab";
  char dataArg[] = "data:meets.txt";
  char* argv[] = { program, dataArg };
  shaykhraziev::U2Args args;

  BOOST_TEST(shaykhraziev::parseU2Args(2, argv, args));
  BOOST_TEST(!args.hasInput);
  BOOST_TEST(args.dataName == "meets.txt");
}

BOOST_AUTO_TEST_CASE(parse_u2_data_and_input_args)
{
  char program[] = "lab";
  char inputArg[] = "in:persons.txt";
  char dataArg[] = "data:meets.txt";
  char* argv[] = { program, inputArg, dataArg };
  shaykhraziev::U2Args args;

  BOOST_TEST(shaykhraziev::parseU2Args(3, argv, args));
  BOOST_TEST(args.hasInput);
  BOOST_TEST(args.inputName == "persons.txt");
  BOOST_TEST(args.dataName == "meets.txt");
}

BOOST_AUTO_TEST_CASE(parse_u2_args_in_any_order)
{
  char program[] = "lab";
  char dataArg[] = "data:meets.txt";
  char inputArg[] = "in:persons.txt";
  char* argv[] = { program, dataArg, inputArg };
  shaykhraziev::U2Args args;

  BOOST_TEST(shaykhraziev::parseU2Args(3, argv, args));
  BOOST_TEST(args.hasInput);
  BOOST_TEST(args.inputName == "persons.txt");
  BOOST_TEST(args.dataName == "meets.txt");
}

BOOST_AUTO_TEST_CASE(reject_u2_args_without_data)
{
  char program[] = "lab";
  char inputArg[] = "in:persons.txt";
  char* argv[] = { program, inputArg };
  shaykhraziev::U2Args args;

  BOOST_TEST(!shaykhraziev::parseU2Args(2, argv, args));
}

BOOST_AUTO_TEST_CASE(reject_repeated_u2_args)
{
  char program[] = "lab";
  char dataArg[] = "data:first.txt";
  char secondDataArg[] = "data:second.txt";
  char* argv[] = { program, dataArg, secondDataArg };
  shaykhraziev::U2Args args;

  BOOST_TEST(!shaykhraziev::parseU2Args(3, argv, args));
}

BOOST_AUTO_TEST_CASE(reject_unknown_u2_arg)
{
  char program[] = "lab";
  char dataArg[] = "data:meets.txt";
  char unknownArg[] = "out:persons.txt";
  char* argv[] = { program, dataArg, unknownArg };
  shaykhraziev::U2Args args;

  BOOST_TEST(!shaykhraziev::parseU2Args(3, argv, args));
}

BOOST_AUTO_TEST_CASE(load_u2_data_with_persons)
{
  const char* personsName = "out/u2-persons.txt";
  const char* dataName = "out/u2-meets.txt";
  writeTextFile(personsName, "31 The Agent\n");
  writeTextFile(dataName, "33 31 10\n33 32 11\n");
  shaykhraziev::U2Args args = { true, personsName, dataName };
  shaykhraziev::U2Storage storage;
  shaykhraziev::initU2Storage(storage);

  BOOST_TEST(shaykhraziev::loadU2Data(args, storage) == 0);
  BOOST_TEST(storage.persons.size == 1);
  BOOST_TEST(storage.meetings.size == 2);
  BOOST_TEST(shaykhraziev::contains(storage.personsById, static_cast< size_t >(31)));
  BOOST_TEST(shaykhraziev::contains(storage.knownIds, static_cast< size_t >(31)));
  BOOST_TEST(shaykhraziev::contains(storage.knownIds, static_cast< size_t >(33)));

  shaykhraziev::clearU2Storage(storage);
  std::remove(personsName);
  std::remove(dataName);
}

BOOST_AUTO_TEST_CASE(load_u2_data_file_open_failure)
{
  shaykhraziev::U2Args args = { false, "", "out/no-such-u2-data.txt" };
  shaykhraziev::U2Storage storage;
  shaykhraziev::initU2Storage(storage);

  BOOST_TEST(shaykhraziev::loadU2Data(args, storage) == 2);

  shaykhraziev::clearU2Storage(storage);
}

BOOST_AUTO_TEST_CASE(load_u2_data_meeting_parse_failure)
{
  const char* dataName = "out/u2-bad-meets.txt";
  writeTextFile(dataName, "33 31 broken\n");
  shaykhraziev::U2Args args = { false, "", dataName };
  shaykhraziev::U2Storage storage;
  shaykhraziev::initU2Storage(storage);

  BOOST_TEST(shaykhraziev::loadU2Data(args, storage) == 3);

  shaykhraziev::clearU2Storage(storage);
  std::remove(dataName);
}
