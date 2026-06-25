#include <exception>
#include <iostream>
#include "commands.hpp"
#include "program.hpp"

int main(int argc, char* argv[])
{
  shaykhraziev::U2Args args;
  if (!shaykhraziev::parseU2Args(argc, argv, args))
  {
    std::cerr << "Invalid arguments\n";
    return 1;
  }
  shaykhraziev::U2Storage storage;
  try
  {
    shaykhraziev::initU2Storage(storage);
    const int loadCode = shaykhraziev::loadU2Data(args, storage);
    if (loadCode == 2)
    {
      std::cerr << "File open error\n";
      shaykhraziev::clearU2Storage(storage);
      return 2;
    }
    if (loadCode == 3)
    {
      std::cerr << "Invalid meeting data\n";
      shaykhraziev::clearU2Storage(storage);
      return 3;
    }
    shaykhraziev::runCommandLoop(storage, std::cin, std::cout);
  }
  catch (const std::exception&)
  {
    shaykhraziev::clearU2Storage(storage);
    std::cerr << "Internal error\n";
    return 2;
  }
  shaykhraziev::clearU2Storage(storage);
  return 0;
}
