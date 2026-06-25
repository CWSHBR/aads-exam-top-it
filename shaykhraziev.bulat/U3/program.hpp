#ifndef U3_PROGRAM_HPP
#define U3_PROGRAM_HPP

#include <string>
#include "../common/domain.hpp"
#include "../common/hash-table.hpp"
#include "../common/list.hpp"

namespace shaykhraziev
{
  struct U3Args
  {
    bool hasInput;
    std::string inputName;
    List< std::string > dataNames;
  };

  struct U3Storage
  {
    List< Person > persons;
    HashTable< size_t, Person* > personsById;
    HashTable< size_t, bool > knownIds;
    List< DatedMeeting > meetings;
    List< Date > dates;
    List< DateRange > ranges;
  };

  void initU3Args(U3Args& args);
  void clearU3Args(U3Args& args);
  bool parseU3Args(int argc, char* argv[], U3Args& args);
  void initU3Storage(U3Storage& storage);
  void clearU3Storage(U3Storage& storage);
  void pushInitialRange(U3Storage& storage);
  int loadU3Data(U3Args& args, U3Storage& storage);
}

#endif
