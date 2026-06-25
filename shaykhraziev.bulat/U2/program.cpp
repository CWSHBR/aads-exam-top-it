#include "program.hpp"
#include <fstream>
#include "../common/meeting-io.hpp"
#include "../common/parse.hpp"
#include "../common/person-io.hpp"

namespace
{
  void addKnownPersons(shaykhraziev::U2Storage& storage)
  {
    shaykhraziev::ListIterator< shaykhraziev::Person > iterator =
        shaykhraziev::begin(storage.persons);
    while (!shaykhraziev::isEnd(iterator))
    {
      const bool known = true;
      shaykhraziev::insert(storage.knownIds, shaykhraziev::get(iterator).id, known);
      iterator = shaykhraziev::next(iterator);
    }
  }
}

bool shaykhraziev::parseU2Args(int argc, char* argv[], U2Args& args)
{
  args.hasInput = false;
  args.inputName.clear();
  args.dataName.clear();
  bool hasData = false;
  if ((argc < 2) || (argc > 3))
  {
    return false;
  }
  for (int i = 1; i < argc; ++i)
  {
    const std::string argument = argv[i];
    std::string filename;
    if (parseFileArg(argument, "in:", filename))
    {
      if (args.hasInput)
      {
        return false;
      }
      args.hasInput = true;
      args.inputName = filename;
    }
    else if (parseFileArg(argument, "data:", filename))
    {
      if (hasData)
      {
        return false;
      }
      hasData = true;
      args.dataName = filename;
    }
    else
    {
      return false;
    }
  }
  return hasData;
}

void shaykhraziev::initU2Storage(U2Storage& storage)
{
  initList(storage.persons);
  initList(storage.meetings);
  initHashTable(storage.personsById, 101, hashSizeT, equalSizeT);
  initHashTable(storage.knownIds, 101, hashSizeT, equalSizeT);
}

void shaykhraziev::clearU2Storage(U2Storage& storage)
{
  clearHashTable(storage.knownIds);
  clearHashTable(storage.personsById);
  clearList(storage.meetings);
  clearList(storage.persons);
}

int shaykhraziev::loadU2Data(const U2Args& args, U2Storage& storage)
{
  if (args.hasInput)
  {
    std::ifstream input(args.inputName.c_str());
    if (!input)
    {
      return 2;
    }
    size_t accepted = 0;
    size_t ignored = 0;
    readPersons(input, storage.persons, storage.personsById, accepted, ignored);
    addKnownPersons(storage);
  }

  std::ifstream data(args.dataName.c_str());
  if (!data)
  {
    return 2;
  }
  if (!readMeetings(data, storage.meetings, storage.knownIds))
  {
    return 3;
  }
  return 0;
}
