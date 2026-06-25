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
