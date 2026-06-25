#ifndef U2_PROGRAM_HPP
#define U2_PROGRAM_HPP

#include <string>
#include "../common/domain.hpp"
#include "../common/hash-table.hpp"
#include "../common/list.hpp"

namespace shaykhraziev
{
  struct U2Args
  {
    bool hasInput;
    std::string inputName;
    std::string dataName;
  };

  struct U2Storage
  {
    List< Person > persons;
    HashTable< size_t, Person* > personsById;
    HashTable< size_t, bool > knownIds;
    List< Meeting > meetings;
  };

  bool parseU2Args(int argc, char* argv[], U2Args& args);
  void initU2Storage(U2Storage& storage);
  void clearU2Storage(U2Storage& storage);
  int loadU2Data(const U2Args& args, U2Storage& storage);
}

#endif
