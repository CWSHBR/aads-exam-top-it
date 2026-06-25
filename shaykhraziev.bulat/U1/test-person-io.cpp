#include <boost/test/unit_test.hpp>
#include <cstdio>
#include <fstream>
#include "../common/domain.hpp"
#include "../common/hash-table.hpp"
#include "../common/list.hpp"
#include "../common/parse.hpp"
#include "../common/person-io.hpp"

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

  void initPersonStorage(shaykhraziev::List< shaykhraziev::Person >& persons,
      shaykhraziev::HashTable< size_t, shaykhraziev::Person* >& personsById)
  {
    shaykhraziev::initList(persons);
    shaykhraziev::initHashTable(personsById, 17, shaykhraziev::hashSizeT, shaykhraziev::equalSizeT);
  }

  void clearPersonStorage(shaykhraziev::List< shaykhraziev::Person >& persons,
      shaykhraziev::HashTable< size_t, shaykhraziev::Person* >& personsById)
  {
    shaykhraziev::clearHashTable(personsById);
    shaykhraziev::clearList(persons);
  }
}

BOOST_AUTO_TEST_CASE(parse_correct_person_line)
{
  const std::string line = "100 Sneaky person";
  size_t id = 0;
  size_t position = 0;
  std::string description;

  BOOST_TEST(shaykhraziev::parseSizeTPrefix(line, id, position));
  BOOST_TEST(id == 100);
  BOOST_TEST(shaykhraziev::readRestDescription(line, position, description));
  BOOST_TEST(description == "Sneaky person");
}

BOOST_AUTO_TEST_CASE(reject_line_without_id)
{
  const std::string line = "abc Sneaky person";
  size_t id = 0;
  size_t position = 0;

  BOOST_TEST(!shaykhraziev::parseSizeTPrefix(line, id, position));
}

BOOST_AUTO_TEST_CASE(reject_empty_description)
{
  const std::string line = "100    ";
  size_t id = 0;
  size_t position = 0;
  std::string description;

  BOOST_TEST(shaykhraziev::parseSizeTPrefix(line, id, position));
  BOOST_TEST(!shaykhraziev::readRestDescription(line, position, description));
}

BOOST_AUTO_TEST_CASE(skip_many_spaces_after_id)
{
  const std::string line = "100 \t  Sneaky person";
  size_t id = 0;
  size_t position = 0;
  std::string description;

  BOOST_TEST(shaykhraziev::parseSizeTPrefix(line, id, position));
  BOOST_TEST(shaykhraziev::readRestDescription(line, position, description));
  BOOST_TEST(description == "Sneaky person");
}

BOOST_AUTO_TEST_CASE(quoted_description_is_plain_text)
{
  const std::string line = "56 \"Scarface\"";
  size_t id = 0;
  size_t position = 0;
  std::string description;

  BOOST_TEST(shaykhraziev::parseSizeTPrefix(line, id, position));
  BOOST_TEST(shaykhraziev::readRestDescription(line, position, description));
  BOOST_TEST(description == "\"Scarface\"");
}

BOOST_AUTO_TEST_CASE(parse_input_file_argument)
{
  std::string filename;

  BOOST_TEST(shaykhraziev::parseFileArg("in:input.txt", "in:", filename));
  BOOST_TEST(filename == "input.txt");
}

BOOST_AUTO_TEST_CASE(parse_output_file_argument)
{
  std::string filename;

  BOOST_TEST(shaykhraziev::parseFileArg("out:output.txt", "out:", filename));
  BOOST_TEST(filename == "output.txt");
}

BOOST_AUTO_TEST_CASE(reject_unknown_argument)
{
  std::string filename;

  BOOST_TEST(!shaykhraziev::parseFileArg("data:input.txt", "in:", filename));
  BOOST_TEST(!shaykhraziev::parseFileArg("data:input.txt", "out:", filename));
}

BOOST_AUTO_TEST_CASE(reject_empty_filename_argument)
{
  std::string filename;

  BOOST_TEST(!shaykhraziev::parseFileArg("in:", "in:", filename));
  BOOST_TEST(!shaykhraziev::parseFileArg("out:", "out:", filename));
}

BOOST_AUTO_TEST_CASE(parse_u1_arguments)
{
  char program[] = "lab";
  char inputArg[] = "in:input.txt";
  char outputArg[] = "out:output.txt";
  char* argv[] = { program, inputArg, outputArg };
  bool hasInput = false;
  bool hasOutput = false;
  std::string input;
  std::string output;

  BOOST_TEST(shaykhraziev::parseU1Args(3, argv, hasInput, input, hasOutput, output));
  BOOST_TEST(hasInput);
  BOOST_TEST(input == "input.txt");
  BOOST_TEST(hasOutput);
  BOOST_TEST(output == "output.txt");
}

BOOST_AUTO_TEST_CASE(reject_repeated_u1_arguments)
{
  char program[] = "lab";
  char firstInputArg[] = "in:first.txt";
  char secondInputArg[] = "in:second.txt";
  char* argv[] = { program, firstInputArg, secondInputArg };
  bool hasInput = false;
  bool hasOutput = false;
  std::string input;
  std::string output;

  BOOST_TEST(!shaykhraziev::parseU1Args(3, argv, hasInput, input, hasOutput, output));
}

BOOST_AUTO_TEST_CASE(reject_unknown_u1_argument)
{
  char program[] = "lab";
  char unknownArg[] = "data:input.txt";
  char* argv[] = { program, unknownArg };
  bool hasInput = false;
  bool hasOutput = false;
  std::string input;
  std::string output;

  BOOST_TEST(!shaykhraziev::parseU1Args(2, argv, hasInput, input, hasOutput, output));
}

BOOST_AUTO_TEST_CASE(person_has_required_fields)
{
  shaykhraziev::Person person = { 1, "info" };

  BOOST_TEST(person.id == 1);
  BOOST_TEST(person.info == "info");
}

BOOST_AUTO_TEST_CASE(add_and_find_person)
{
  shaykhraziev::List< shaykhraziev::Person > persons;
  shaykhraziev::HashTable< size_t, shaykhraziev::Person* > personsById;
  initPersonStorage(persons, personsById);
  const shaykhraziev::Person person = { 10, "Known" };

  BOOST_TEST(shaykhraziev::addPerson(persons, personsById, person));
  shaykhraziev::Person* found = shaykhraziev::findPersonById(personsById, 10);

  BOOST_TEST(found != nullptr);
  BOOST_TEST(found->id == 10);
  BOOST_TEST(found->info == "Known");

  clearPersonStorage(persons, personsById);
}

BOOST_AUTO_TEST_CASE(reject_duplicate_person)
{
  shaykhraziev::List< shaykhraziev::Person > persons;
  shaykhraziev::HashTable< size_t, shaykhraziev::Person* > personsById;
  initPersonStorage(persons, personsById);
  const shaykhraziev::Person first = { 10, "Known" };
  const shaykhraziev::Person second = { 10, "Other" };

  BOOST_TEST(shaykhraziev::addPerson(persons, personsById, first));
  BOOST_TEST(!shaykhraziev::addPerson(persons, personsById, second));
  BOOST_TEST(persons.size == 1);

  clearPersonStorage(persons, personsById);
}

BOOST_AUTO_TEST_CASE(read_persons_full_example)
{
  const char* inputName = "out/u1-read-persons-input.txt";
  writeTextFile(inputName,
      "100 Sneaky person\n"
      "23 Deadman\n"
      "31 The Agent\n"
      "56 \"Scarface\"\n"
      "99\n"
      "31 Mr. Bond\n");
  std::ifstream input(inputName);
  shaykhraziev::List< shaykhraziev::Person > persons;
  shaykhraziev::HashTable< size_t, shaykhraziev::Person* > personsById;
  initPersonStorage(persons, personsById);
  size_t accepted = 0;
  size_t ignored = 0;

  shaykhraziev::readPersons(input, persons, personsById, accepted, ignored);

  BOOST_TEST(accepted == 4);
  BOOST_TEST(ignored == 2);
  BOOST_TEST(persons.size == 4);
  BOOST_TEST(shaykhraziev::findPersonById(personsById, 31)->info == "The Agent");

  clearPersonStorage(persons, personsById);
  std::remove(inputName);
}

BOOST_AUTO_TEST_CASE(write_persons_preserves_order)
{
  const char* outputName = "out/u1-write-persons-output.txt";
  shaykhraziev::List< shaykhraziev::Person > persons;
  shaykhraziev::HashTable< size_t, shaykhraziev::Person* > personsById;
  initPersonStorage(persons, personsById);
  shaykhraziev::addPerson(persons, personsById, shaykhraziev::Person{ 100, "Sneaky person" });
  shaykhraziev::addPerson(persons, personsById, shaykhraziev::Person{ 23, "Deadman" });
  shaykhraziev::addPerson(persons, personsById, shaykhraziev::Person{ 31, "The Agent" });
  std::ofstream output(outputName);

  shaykhraziev::writePersons(output, persons);
  output.close();

  BOOST_TEST(readTextFile(outputName) == "100 Sneaky person\n23 Deadman\n31 The Agent\n");

  clearPersonStorage(persons, personsById);
  std::remove(outputName);
}

BOOST_AUTO_TEST_CASE(write_empty_persons_outputs_newline)
{
  const char* outputName = "out/u1-write-empty-persons-output.txt";
  shaykhraziev::List< shaykhraziev::Person > persons;
  shaykhraziev::initList(persons);
  std::ofstream output(outputName);

  shaykhraziev::writePersons(output, persons);
  output.close();

  BOOST_TEST(readTextFile(outputName) == "\n");

  shaykhraziev::clearList(persons);
  std::remove(outputName);
}

BOOST_AUTO_TEST_CASE(read_persons_ignores_bad_lines)
{
  const char* inputName = "out/u1-read-bad-persons-input.txt";
  writeTextFile(inputName,
      "abc Broken\n"
      "100\n"
      "100 Known\n"
      "100 Duplicate\n");
  std::ifstream input(inputName);
  shaykhraziev::List< shaykhraziev::Person > persons;
  shaykhraziev::HashTable< size_t, shaykhraziev::Person* > personsById;
  initPersonStorage(persons, personsById);
  size_t accepted = 0;
  size_t ignored = 0;

  shaykhraziev::readPersons(input, persons, personsById, accepted, ignored);

  BOOST_TEST(accepted == 1);
  BOOST_TEST(ignored == 3);
  BOOST_TEST(persons.size == 1);
  BOOST_TEST(shaykhraziev::findPersonById(personsById, 100)->info == "Known");

  clearPersonStorage(persons, personsById);
  std::remove(inputName);
}

BOOST_AUTO_TEST_CASE(read_persons_skips_empty_lines)
{
  const char* inputName = "out/u1-read-empty-lines-input.txt";
  writeTextFile(inputName,
      "\n"
      "   \t\n"
      "31 Mr. Bond\n"
      "\n");
  std::ifstream input(inputName);
  shaykhraziev::List< shaykhraziev::Person > persons;
  shaykhraziev::HashTable< size_t, shaykhraziev::Person* > personsById;
  initPersonStorage(persons, personsById);
  size_t accepted = 0;
  size_t ignored = 0;

  shaykhraziev::readPersons(input, persons, personsById, accepted, ignored);

  BOOST_TEST(accepted == 1);
  BOOST_TEST(ignored == 0);
  BOOST_TEST(persons.size == 1);

  clearPersonStorage(persons, personsById);
  std::remove(inputName);
}
