#ifndef PERSON_IO_HPP
#define PERSON_IO_HPP

#include <cstddef>
#include <iosfwd>
#include "../common/domain.hpp"
#include "../common/hash-table.hpp"
#include "../common/list.hpp"

namespace shaykhraziev
{
  Person* findPersonById(HashTable< size_t, Person* >& personsById, size_t id);
  bool addPerson(List< Person >& persons, HashTable< size_t, Person* >& personsById,
      const Person& person);
  void readPersons(std::istream& input,
      List< Person >& persons,
      HashTable< size_t, Person* >& personsById,
      size_t& accepted,
      size_t& ignored);
  void writePersons(std::ostream& output, List< Person >& persons);
}

#endif
