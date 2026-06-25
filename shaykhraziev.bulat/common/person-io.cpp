#include "../common/person-io.hpp"
#include <istream>
#include <ostream>
#include "../common/parse.hpp"

shaykhraziev::Person* shaykhraziev::findPersonById(HashTable< size_t, Person* >& personsById,
    size_t id)
{
  Person** person = find(personsById, id);
  if (person == nullptr)
  {
    return nullptr;
  }
  return *person;
}

bool shaykhraziev::addPerson(List< Person >& persons,
    HashTable< size_t, Person* >& personsById,
    const Person& person)
{
  if (findPersonById(personsById, person.id) != nullptr)
  {
    return false;
  }
  Person* inserted = pushBack(persons, person);
  try
  {
    if (!insert(personsById, person.id, inserted))
    {
      popBack(persons);
      return false;
    }
  }
  catch (...)
  {
    popBack(persons);
    throw;
  }
  return true;
}

void shaykhraziev::readPersons(std::istream& input,
    List< Person >& persons,
    HashTable< size_t, Person* >& personsById,
    size_t& accepted,
    size_t& ignored)
{
  accepted = 0;
  ignored = 0;
  std::string line;
  while (std::getline(input, line))
  {
    if (skipSpaces(line, 0) == line.size())
    {
      continue;
    }
    size_t id = 0;
    size_t position = 0;
    std::string description;
    if (!parseSizeTPrefix(line, id, position) || !readRestDescription(line, position, description))
    {
      ++ignored;
    }
    else
    {
      const Person person = { id, description };
      if (addPerson(persons, personsById, person))
      {
        ++accepted;
      }
      else
      {
        ++ignored;
      }
    }
  }
}

void shaykhraziev::writePersons(std::ostream& output, List< Person >& persons)
{
  ListIterator< Person > iterator = begin(persons);
  if (!isEnd(iterator))
  {
    const Person& person = get(iterator);
    output << person.id << ' ' << person.info;
    iterator = next(iterator);
  }
  while (!isEnd(iterator))
  {
    const Person& person = get(iterator);
    output << '\n' << person.id << ' ' << person.info;
    iterator = next(iterator);
  }
  output << '\n';
}
