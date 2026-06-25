#include <exception>
#include <fstream>
#include <iostream>
#include "../common/domain.hpp"
#include "../common/hash-table.hpp"
#include "../common/list.hpp"
#include "../common/parse.hpp"
#include "../common/person-io.hpp"

int main(int argc, char* argv[])
{
  bool hasInput = false;
  bool hasOutput = false;
  std::string inputName;
  std::string outputName;
  if (shaykhraziev::hasTooManyU1Args(argc))
  {
    return 0;
  }
  if (!shaykhraziev::parseU1Args(argc, argv, hasInput, inputName, hasOutput, outputName))
  {
    return 1;
  }
  shaykhraziev::List< shaykhraziev::Person > persons;
  shaykhraziev::HashTable< size_t, shaykhraziev::Person* > personsById;
  shaykhraziev::initList(persons);
  try
  {
    shaykhraziev::initHashTable(personsById,
        101,
        shaykhraziev::hashSizeT,
        shaykhraziev::equalSizeT);
    std::ifstream inputFile;
    std::istream* input = &std::cin;
    if (hasInput)
    {
      inputFile.open(inputName.c_str());
      if (!inputFile)
      {
        shaykhraziev::clearHashTable(personsById);
        shaykhraziev::clearList(persons);
        return 2;
      }
      input = &inputFile;
    }

    size_t accepted = 0;
    size_t ignored = 0;
    shaykhraziev::readPersons(*input, persons, personsById, accepted, ignored);
    inputFile.close();

    std::ofstream outputFile;
    std::ostream* output = &std::cout;
    if (hasOutput)
    {
      outputFile.open(outputName.c_str());
      if (!outputFile)
      {
        shaykhraziev::clearHashTable(personsById);
        shaykhraziev::clearList(persons);
        return 2;
      }
      output = &outputFile;
    }
    shaykhraziev::writePersons(*output, persons);
    std::cerr << accepted << ' ' << ignored << '\n';
  }
  catch (const std::exception&)
  {
    shaykhraziev::clearHashTable(personsById);
    shaykhraziev::clearList(persons);
    return 2;
  }
  shaykhraziev::clearHashTable(personsById);
  shaykhraziev::clearList(persons);
  return 0;
}
