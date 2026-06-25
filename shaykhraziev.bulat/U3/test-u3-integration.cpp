#include <boost/test/unit_test.hpp>
#include <cstdio>
#include <fstream>
#include "commands.hpp"

namespace
{
  void writeTextFile(const char* filename, const char* text)
  {
    std::ofstream output(filename);
    output << text;
  }

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
}

BOOST_AUTO_TEST_CASE(command_loop_full_u3_scenario)
{
  const char* personsName = "out/u3-integration-persons.txt";
  const char* firstData = "out/u3-integration-first.txt";
  const char* secondData = "out/u3-integration-second.txt";
  const char* commandsName = "out/u3-integration-commands.txt";
  const char* outputName = "out/u3-integration-output.txt";
  writeTextFile(personsName, "31 Mr. Bond\n100 Sneaky person\n");
  writeTextFile(firstData, "1 1 2024 31 33 9\n10 1 2024 41 33 10\n");
  writeTextFile(secondData, "20 1 2024 41 32 20\n20 1 2024 31 32 99\n");
  writeTextFile(commandsName,
      "range\n"
      "after 10 1 2024\n"
      "range\n"
      "meet 31\n"
      "pop-range\n"
      "less 10 31\n"
      "unknown\n"
      "desc 33 \"Agent 007\"\n"
      "desc 33\n");
  char arg0[] = "lab";
  char arg1[] = "in:out/u3-integration-persons.txt";
  char arg2[] = "data:out/u3-integration-first.txt";
  char arg3[] = "data:out/u3-integration-second.txt";
  char* argv[] = { arg0, arg1, arg2, arg3 };
  shaykhraziev::U3Args args;
  shaykhraziev::U3Storage storage;
  shaykhraziev::initU3Args(args);
  shaykhraziev::initU3Storage(storage);
  std::ifstream commands(commandsName);
  std::ofstream output(outputName);

  BOOST_TEST(shaykhraziev::parseU3Args(4, argv, args));
  BOOST_TEST(shaykhraziev::loadU3Data(args, storage) == 0);
  shaykhraziev::runCommandLoop(storage, commands, output);
  output.close();
  BOOST_TEST(readTextFile(outputName) ==
      "1 1 2024 : 20 1 2024\n"
      "10 1 2024 : 20 1 2024\n"
      "32 99\n"
      "33\n"
      "<INVALID COMMAND>\n"
      "Agent 007\n");

  shaykhraziev::clearU3Storage(storage);
  shaykhraziev::clearU3Args(args);
  std::remove(personsName);
  std::remove(firstData);
  std::remove(secondData);
  std::remove(commandsName);
  std::remove(outputName);
}

BOOST_AUTO_TEST_CASE(command_loop_eof_without_commands)
{
  shaykhraziev::U3Storage storage;
  shaykhraziev::initU3Storage(storage);
  shaykhraziev::pushInitialRange(storage);
  const char* inputName = "out/u3-empty-command-input.txt";
  const char* outputName = "out/u3-empty-command-output.txt";
  writeTextFile(inputName, "");
  std::ifstream input(inputName);
  std::ofstream output(outputName);

  shaykhraziev::runCommandLoop(storage, input, output);
  output.close();
  BOOST_TEST(readTextFile(outputName).empty());

  shaykhraziev::clearU3Storage(storage);
  std::remove(inputName);
  std::remove(outputName);
}

BOOST_AUTO_TEST_CASE(load_invalid_looking_dates_and_meet_after_range)
{
  const char* personsName = "out/u3-invalid-looking-persons.txt";
  const char* firstData = "out/u3-invalid-looking-first-data.txt";
  const char* secondData = "out/u3-invalid-looking-second-data.txt";
  const char* commandsName = "out/u3-invalid-looking-commands.txt";
  const char* outputName = "out/u3-invalid-looking-output.txt";
  writeTextFile(personsName, "31 Mr. Bond\n32 Some person\n41 Other person\n");
  writeTextFile(firstData,
      "5 5 1005 33 31 5\n"
      "9 99 1700 33 31 10\n");
  writeTextFile(secondData,
      "11 33 1945 33 32 11\n"
      "12 33 1946 33 41 10\n");
  writeTextFile(commandsName, "after 10 10 1010\nmeets 33\n");
  char arg0[] = "lab";
  char arg1[] = "in:out/u3-invalid-looking-persons.txt";
  char arg2[] = "data:out/u3-invalid-looking-first-data.txt";
  char arg3[] = "data:out/u3-invalid-looking-second-data.txt";
  char* argv[] = { arg0, arg1, arg2, arg3 };
  shaykhraziev::U3Args args;
  shaykhraziev::U3Storage storage;
  shaykhraziev::initU3Args(args);
  shaykhraziev::initU3Storage(storage);
  std::ifstream commands(commandsName);
  std::ofstream output(outputName);

  BOOST_TEST(shaykhraziev::parseU3Args(4, argv, args));
  BOOST_TEST(shaykhraziev::loadU3Data(args, storage) == 0);
  shaykhraziev::runCommandLoop(storage, commands, output);
  output.close();
  BOOST_TEST(readTextFile(outputName) == "31 10\n32 11\n41 10\n");

  shaykhraziev::clearU3Storage(storage);
  shaykhraziev::clearU3Args(args);
  std::remove(personsName);
  std::remove(firstData);
  std::remove(secondData);
  std::remove(commandsName);
  std::remove(outputName);
}

BOOST_AUTO_TEST_CASE(initial_range_uses_dates_from_all_data_files)
{
  const char* firstData = "out/u3-range-first-data.txt";
  const char* secondData = "out/u3-range-second-data.txt";
  const char* commandsName = "out/u3-range-all-commands.txt";
  const char* outputName = "out/u3-range-all-output.txt";
  writeTextFile(firstData,
      "5 5 1005 33 31 10\n"
      "12 33 1946 33 41 10\n");
  writeTextFile(secondData,
      "9 99 1700 33 32 11\n"
      "11 33 1945 32 31 99\n");
  writeTextFile(commandsName, "range\n");
  char arg0[] = "lab";
  char arg1[] = "data:out/u3-range-first-data.txt";
  char arg2[] = "data:out/u3-range-second-data.txt";
  char* argv[] = { arg0, arg1, arg2 };
  shaykhraziev::U3Args args;
  shaykhraziev::U3Storage storage;
  shaykhraziev::initU3Args(args);
  shaykhraziev::initU3Storage(storage);
  std::ifstream commands(commandsName);
  std::ofstream output(outputName);

  BOOST_TEST(shaykhraziev::parseU3Args(3, argv, args));
  BOOST_TEST(shaykhraziev::loadU3Data(args, storage) == 0);
  shaykhraziev::runCommandLoop(storage, commands, output);
  output.close();
  BOOST_TEST(readTextFile(outputName) == "5 5 1005 : 12 33 1946\n");

  shaykhraziev::clearU3Storage(storage);
  shaykhraziev::clearU3Args(args);
  std::remove(firstData);
  std::remove(secondData);
  std::remove(commandsName);
  std::remove(outputName);
}

BOOST_AUTO_TEST_CASE(after_empty_range_then_next_after_invalid)
{
  const char* dataName = "out/u3-after-empty-data.txt";
  const char* commandsName = "out/u3-after-empty-commands.txt";
  const char* outputName = "out/u3-after-empty-output.txt";
  writeTextFile(dataName,
      "5 5 1005 33 31 10\n"
      "9 99 1700 33 32 11\n"
      "12 33 1946 33 41 10\n");
  writeTextFile(commandsName,
      "after 65 832 1889\n"
      "range\n"
      "after 10 10 1010\n");
  char arg0[] = "lab";
  char arg1[] = "data:out/u3-after-empty-data.txt";
  char* argv[] = { arg0, arg1 };
  shaykhraziev::U3Args args;
  shaykhraziev::U3Storage storage;
  shaykhraziev::initU3Args(args);
  shaykhraziev::initU3Storage(storage);
  std::ifstream commands(commandsName);
  std::ofstream output(outputName);

  BOOST_TEST(shaykhraziev::parseU3Args(2, argv, args));
  BOOST_TEST(shaykhraziev::loadU3Data(args, storage) == 0);
  shaykhraziev::runCommandLoop(storage, commands, output);
  output.close();
  BOOST_TEST(readTextFile(outputName) == "<EMPTY>\n<INVALID COMMAND>\n");

  shaykhraziev::clearU3Storage(storage);
  shaykhraziev::clearU3Args(args);
  std::remove(dataName);
  std::remove(commandsName);
  std::remove(outputName);
}

BOOST_AUTO_TEST_CASE(before_empty_and_pop_restore_initial_range)
{
  const char* dataName = "out/u3-before-empty-data.txt";
  const char* commandsName = "out/u3-before-empty-commands.txt";
  const char* outputName = "out/u3-before-empty-output.txt";
  writeTextFile(dataName,
      "5 5 1005 33 31 10\n"
      "12 33 1946 33 41 10\n");
  writeTextFile(commandsName,
      "before 5 5 1004\n"
      "range\n"
      "pop-range\n"
      "range\n");
  char arg0[] = "lab";
  char arg1[] = "data:out/u3-before-empty-data.txt";
  char* argv[] = { arg0, arg1 };
  shaykhraziev::U3Args args;
  shaykhraziev::U3Storage storage;
  shaykhraziev::initU3Args(args);
  shaykhraziev::initU3Storage(storage);
  std::ifstream commands(commandsName);
  std::ofstream output(outputName);

  BOOST_TEST(shaykhraziev::parseU3Args(2, argv, args));
  BOOST_TEST(shaykhraziev::loadU3Data(args, storage) == 0);
  shaykhraziev::runCommandLoop(storage, commands, output);
  output.close();
  BOOST_TEST(readTextFile(outputName) == "<EMPTY>\n5 5 1005 : 12 33 1946\n");

  shaykhraziev::clearU3Storage(storage);
  shaykhraziev::clearU3Args(args);
  std::remove(dataName);
  std::remove(commandsName);
  std::remove(outputName);
}

BOOST_AUTO_TEST_CASE(pop_range_restores_previous_non_empty_range)
{
  const char* dataName = "out/u3-pop-previous-data.txt";
  const char* commandsName = "out/u3-pop-previous-commands.txt";
  const char* outputName = "out/u3-pop-previous-output.txt";
  writeTextFile(dataName,
      "5 5 1005 33 31 10\n"
      "9 99 1700 33 32 11\n"
      "11 33 1945 33 41 10\n"
      "12 33 1946 32 41 20\n");
  writeTextFile(commandsName,
      "after 11 10 1010\n"
      "before 12 33 1945\n"
      "range\n"
      "pop-range\n"
      "range\n");
  char arg0[] = "lab";
  char arg1[] = "data:out/u3-pop-previous-data.txt";
  char* argv[] = { arg0, arg1 };
  shaykhraziev::U3Args args;
  shaykhraziev::U3Storage storage;
  shaykhraziev::initU3Args(args);
  shaykhraziev::initU3Storage(storage);
  std::ifstream commands(commandsName);
  std::ofstream output(outputName);

  BOOST_TEST(shaykhraziev::parseU3Args(2, argv, args));
  BOOST_TEST(shaykhraziev::loadU3Data(args, storage) == 0);
  shaykhraziev::runCommandLoop(storage, commands, output);
  output.close();
  BOOST_TEST(readTextFile(outputName) ==
      "9 99 1700 : 11 33 1945\n"
      "9 99 1700 : 12 33 1946\n");

  shaykhraziev::clearU3Storage(storage);
  shaykhraziev::clearU3Args(args);
  std::remove(dataName);
  std::remove(commandsName);
  std::remove(outputName);
}
