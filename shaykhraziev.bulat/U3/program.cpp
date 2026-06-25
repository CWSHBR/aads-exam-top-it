#include "program.hpp"
#include <fstream>
#include "../common/dated-meeting-io.hpp"
#include "../common/parse.hpp"
#include "../common/person-io.hpp"

namespace
{
  void addKnownPersons(shaykhraziev::U3Storage& storage)
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

void shaykhraziev::initU3Args(U3Args& args)
{
  args.hasInput = false;
  args.inputName.clear();
  initList(args.dataNames);
}

void shaykhraziev::clearU3Args(U3Args& args)
{
  clearList(args.dataNames);
  args.hasInput = false;
  args.inputName.clear();
}

bool shaykhraziev::parseU3Args(int argc, char* argv[], U3Args& args)
{
  args.hasInput = false;
  args.inputName.clear();
  clearList(args.dataNames);
  initList(args.dataNames);
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
      pushBack(args.dataNames, filename);
    }
    else
    {
      return false;
    }
  }
  return true;
}

void shaykhraziev::initU3Storage(U3Storage& storage)
{
  initList(storage.persons);
  initList(storage.meetings);
  initList(storage.dates);
  initList(storage.ranges);
  initHashTable(storage.personsById, 101, hashSizeT, equalSizeT);
  initHashTable(storage.knownIds, 101, hashSizeT, equalSizeT);
}

void shaykhraziev::clearU3Storage(U3Storage& storage)
{
  clearList(storage.ranges);
  clearList(storage.dates);
  clearList(storage.meetings);
  clearHashTable(storage.knownIds);
  clearHashTable(storage.personsById);
  clearList(storage.persons);
}

void shaykhraziev::pushInitialRange(U3Storage& storage)
{
  if (storage.dates.size == 0)
  {
    const DateRange range = { Date{ 0, 0, 0 }, Date{ 0, 0, 0 }, true };
    pushBack(storage.ranges, range);
  }
  else
  {
    const DateRange range = { storage.dates.head->value, storage.dates.tail->value, false };
    pushBack(storage.ranges, range);
  }
}

int shaykhraziev::loadU3Data(U3Args& args, U3Storage& storage)
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

  ListIterator< std::string > iterator = begin(args.dataNames);
  while (!isEnd(iterator))
  {
    std::ifstream data(get(iterator).c_str());
    if (!data)
    {
      return 2;
    }
    if (!readDatedMeetings(data, storage.meetings, storage.knownIds, storage.dates))
    {
      return 3;
    }
    iterator = next(iterator);
  }
  pushInitialRange(storage);
  return 0;
}
