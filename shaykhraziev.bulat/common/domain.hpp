#ifndef DOMAIN_HPP
#define DOMAIN_HPP

#include <cstddef>
#include <string>

namespace shaykhraziev
{
  struct Person
  {
    size_t id;
    std::string info;
  };

  struct Meeting
  {
    size_t first;
    size_t second;
    size_t duration;
  };

  struct MeetView
  {
    size_t id;
    size_t duration;
  };
}

#endif
