#include <exception>
#include <iostream>
#include "commands.hpp"
#include "program.hpp"

int main(int argc, char* argv[])
{
  shaykhraziev::U3Args args;
  shaykhraziev::U3Storage storage;
  shaykhraziev::initU3Args(args);
  shaykhraziev::initU3Storage(storage);
  try
  {
    if (!shaykhraziev::parseU3Args(argc, argv, args))
    {
      std::cerr << "Invalid arguments\n";
      shaykhraziev::clearU3Storage(storage);
      shaykhraziev::clearU3Args(args);
      return 1;
    }
    const int loadCode = shaykhraziev::loadU3Data(args, storage);
    if (loadCode == 2)
    {
      std::cerr << "File open error\n";
      shaykhraziev::clearU3Storage(storage);
      shaykhraziev::clearU3Args(args);
      return 2;
    }
    if (loadCode == 3)
    {
      std::cerr << "Invalid meeting data\n";
      shaykhraziev::clearU3Storage(storage);
      shaykhraziev::clearU3Args(args);
      return 3;
    }
    shaykhraziev::runCommandLoop(storage, std::cin, std::cout);
  }
  catch (const std::exception&)
  {
    shaykhraziev::clearU3Storage(storage);
    shaykhraziev::clearU3Args(args);
    std::cerr << "Internal error\n";
    return 2;
  }
  shaykhraziev::clearU3Storage(storage);
  shaykhraziev::clearU3Args(args);
  return 0;
}
