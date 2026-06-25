#include "commands.hpp"
#include <fstream>
#include <istream>
#include <ostream>
#include "../common/ordered-list.hpp"
#include "../common/parse.hpp"
#include "../common/person-io.hpp"

namespace
{
  struct AnonsData
  {
    shaykhraziev::U2Storage* storage;
    shaykhraziev::List< size_t >* result;
  };

  void collectAnonId(shaykhraziev::HashEntry< size_t, bool >& entry, void* data)
  {
    AnonsData* anons = static_cast< AnonsData* >(data);
    if (!shaykhraziev::contains(anons->storage->personsById, entry.key))
    {
      shaykhraziev::insertOrderedUniqueSizeT(*anons->result, entry.key);
    }
  }

  bool parseCommandPrefix(const std::string& line, const char* command, size_t& position)
  {
    if (!shaykhraziev::startsWith(line, command))
    {
      return false;
    }
    position = 0;
    while (command[position] != '\0')
    {
      ++position;
    }
    if ((position < line.size()) && (line[position] != ' ') && (line[position] != '\t'))
    {
      return false;
    }
    return true;
  }

  bool parseExactCommand(const std::string& line, const char* command)
  {
    size_t position = 0;
    if (!parseCommandPrefix(line, command, position))
    {
      return false;
    }
    return shaykhraziev::skipSpaces(line, position) == line.size();
  }

  bool parseIdCommand(const std::string& line, const char* command, size_t& id)
  {
    size_t position = 0;
    if (!parseCommandPrefix(line, command, position))
    {
      return false;
    }
    if (!shaykhraziev::parseSizeTPrefix(line, id, position))
    {
      return false;
    }
    return shaykhraziev::skipSpaces(line, position) == line.size();
  }

  bool parseTimeIdCommand(const std::string& line, const char* command, size_t& time, size_t& id)
  {
    size_t position = 0;
    if (!parseCommandPrefix(line, command, position))
    {
      return false;
    }
    if (!shaykhraziev::parseSizeTPrefix(line, time, position))
    {
      return false;
    }
    if (!shaykhraziev::parseSizeTPrefix(line, id, position))
    {
      return false;
    }
    return shaykhraziev::skipSpaces(line, position) == line.size();
  }

  bool parseTwoIdCommand(const std::string& line, const char* command, size_t& first, size_t& second)
  {
    size_t position = 0;
    if (!parseCommandPrefix(line, command, position))
    {
      return false;
    }
    if (!shaykhraziev::parseSizeTPrefix(line, first, position))
    {
      return false;
    }
    if (!shaykhraziev::parseSizeTPrefix(line, second, position))
    {
      return false;
    }
    return shaykhraziev::skipSpaces(line, position) == line.size();
  }

  bool parseFileCommand(const std::string& line, const char* command, std::string& filename)
  {
    size_t position = 0;
    if (!parseCommandPrefix(line, command, position))
    {
      return false;
    }
    position = shaykhraziev::skipSpaces(line, position);
    if (position >= line.size())
    {
      return false;
    }
    filename = line.substr(position);
    return true;
  }

  bool parseDescriptionCommand(const std::string& line,
      const char* command,
      size_t& id,
      std::string& description)
  {
    size_t position = 0;
    if (!parseCommandPrefix(line, command, position))
    {
      return false;
    }
    if (!shaykhraziev::parseSizeTPrefix(line, id, position))
    {
      return false;
    }
    position = shaykhraziev::skipSpaces(line, position);
    if ((position >= line.size()) || (line[position] != '"'))
    {
      return false;
    }
    ++position;
    const size_t begin = position;
    while ((position < line.size()) && (line[position] != '"'))
    {
      ++position;
    }
    if (position >= line.size())
    {
      return false;
    }
    description = line.substr(begin, position - begin);
    ++position;
    return shaykhraziev::skipSpaces(line, position) == line.size();
  }

  bool shouldUseMeeting(size_t duration, char mode, size_t time)
  {
    return (mode == 'a') || ((mode == 'l') && (duration < time))
        || ((mode == 'g') && (duration > time));
  }

  void collectMeetViews(shaykhraziev::U2Storage& storage,
      size_t id,
      char mode,
      size_t time,
      shaykhraziev::List< shaykhraziev::MeetView >& views)
  {
    shaykhraziev::ListIterator< shaykhraziev::Meeting > iterator =
        shaykhraziev::begin(storage.meetings);
    while (!shaykhraziev::isEnd(iterator))
    {
      const shaykhraziev::Meeting& meeting = shaykhraziev::get(iterator);
      if ((meeting.first == id) && shouldUseMeeting(meeting.duration, mode, time))
      {
        const shaykhraziev::MeetView view = { meeting.second, meeting.duration };
        shaykhraziev::insertOrderedMeetView(views, view);
      }
      else if ((meeting.second == id) && shouldUseMeeting(meeting.duration, mode, time))
      {
        const shaykhraziev::MeetView view = { meeting.first, meeting.duration };
        shaykhraziev::insertOrderedMeetView(views, view);
      }
      iterator = shaykhraziev::next(iterator);
    }
  }

  void collectDurationViews(shaykhraziev::U2Storage& storage,
      size_t id,
      char mode,
      size_t time,
      shaykhraziev::List< shaykhraziev::MeetView >& views)
  {
    shaykhraziev::ListIterator< shaykhraziev::Meeting > iterator =
        shaykhraziev::begin(storage.meetings);
    while (!shaykhraziev::isEnd(iterator))
    {
      const shaykhraziev::Meeting& meeting = shaykhraziev::get(iterator);
      if ((meeting.first == id) && shouldUseMeeting(meeting.duration, mode, time))
      {
        const shaykhraziev::MeetView view = { meeting.second, meeting.duration };
        shaykhraziev::insertOrderedDurationView(views, view);
      }
      else if ((meeting.second == id) && shouldUseMeeting(meeting.duration, mode, time))
      {
        const shaykhraziev::MeetView view = { meeting.first, meeting.duration };
        shaykhraziev::insertOrderedDurationView(views, view);
      }
      iterator = shaykhraziev::next(iterator);
    }
  }

  void printMeetViews(shaykhraziev::List< shaykhraziev::MeetView >& views, std::ostream& output)
  {
    if (views.size == 0)
    {
      output << '\n';
      return;
    }
    shaykhraziev::ListIterator< shaykhraziev::MeetView > iterator = shaykhraziev::begin(views);
    while (!shaykhraziev::isEnd(iterator))
    {
      const shaykhraziev::MeetView& view = shaykhraziev::get(iterator);
      output << view.id << ' ' << view.duration << '\n';
      iterator = shaykhraziev::next(iterator);
    }
  }

  bool executeMeetQuery(shaykhraziev::U2Storage& storage,
      size_t id,
      char mode,
      size_t time,
      std::ostream& output)
  {
    if (!shaykhraziev::contains(storage.knownIds, id))
    {
      return false;
    }
    shaykhraziev::List< shaykhraziev::MeetView > views;
    shaykhraziev::initList(views);
    collectMeetViews(storage, id, mode, time, views);
    printMeetViews(views, output);
    shaykhraziev::clearList(views);
    return true;
  }

  void printMeetIds(shaykhraziev::List< shaykhraziev::MeetView >& views, std::ostream& output)
  {
    if (views.size == 0)
    {
      output << '\n';
      return;
    }
    shaykhraziev::ListIterator< shaykhraziev::MeetView > iterator = shaykhraziev::begin(views);
    while (!shaykhraziev::isEnd(iterator))
    {
      output << shaykhraziev::get(iterator).id << '\n';
      iterator = shaykhraziev::next(iterator);
    }
  }

  bool executeDurationQuery(shaykhraziev::U2Storage& storage,
      size_t id,
      char mode,
      size_t time,
      std::ostream& output)
  {
    if (!shaykhraziev::contains(storage.knownIds, id))
    {
      return false;
    }
    shaykhraziev::List< shaykhraziev::MeetView > views;
    shaykhraziev::initList(views);
    collectDurationViews(storage, id, mode, time, views);
    printMeetIds(views, output);
    shaykhraziev::clearList(views);
    return true;
  }

  size_t getOtherId(const shaykhraziev::Meeting& meeting, size_t id)
  {
    if (meeting.first == id)
    {
      return meeting.second;
    }
    return meeting.first;
  }

  bool hasMeetingWith(const shaykhraziev::Meeting& meeting, size_t id)
  {
    return (meeting.first == id) || (meeting.second == id);
  }

  void printSizeTList(shaykhraziev::List< size_t >& values, std::ostream& output)
  {
    if (values.size == 0)
    {
      output << '\n';
      return;
    }
    shaykhraziev::ListIterator< size_t > iterator = shaykhraziev::begin(values);
    while (!shaykhraziev::isEnd(iterator))
    {
      output << shaykhraziev::get(iterator) << '\n';
      iterator = shaykhraziev::next(iterator);
    }
  }
}

bool shaykhraziev::executeAnons(U2Storage& storage, std::ostream& output)
{
  List< size_t > anons;
  initList(anons);
  AnonsData data = { &storage, &anons };
  forEachEntry(storage.knownIds, collectAnonId, &data);
  printSizeTList(anons, output);
  clearList(anons);
  return true;
}

bool shaykhraziev::executeDesc(U2Storage& storage, const std::string& line, std::ostream& output)
{
  size_t id = 0;
  std::string description;
  if (parseDescriptionCommand(line, "desc", id, description))
  {
    if (!contains(storage.knownIds, id))
    {
      return false;
    }
    Person* person = findPersonById(storage.personsById, id);
    if (person != nullptr)
    {
      person->info = description;
      return true;
    }
    const Person newPerson = { id, description };
    return addPerson(storage.persons, storage.personsById, newPerson);
  }
  if (!parseIdCommand(line, "desc", id) || !contains(storage.knownIds, id))
  {
    return false;
  }
  Person* person = findPersonById(storage.personsById, id);
  if (person == nullptr)
  {
    output << "<ANON>\n";
  }
  else
  {
    output << person->info << '\n';
  }
  return true;
}

bool shaykhraziev::executeRedesc(U2Storage& storage, const std::string& line)
{
  size_t id = 0;
  std::string description;
  if (!parseDescriptionCommand(line, "redesc", id, description) || !contains(storage.knownIds, id))
  {
    return false;
  }
  Person* person = findPersonById(storage.personsById, id);
  if (person != nullptr)
  {
    person->info = description;
    return true;
  }
  const Person newPerson = { id, description };
  return addPerson(storage.persons, storage.personsById, newPerson);
}

bool shaykhraziev::executeMeets(U2Storage& storage, const std::string& line, std::ostream& output)
{
  size_t id = 0;
  if (!parseIdCommand(line, "meet", id) && !parseIdCommand(line, "meets", id))
  {
    return false;
  }
  return executeMeetQuery(storage, id, 'a', 0, output);
}

bool shaykhraziev::executeLess(U2Storage& storage, const std::string& line, std::ostream& output)
{
  size_t time = 0;
  size_t id = 0;
  if (!parseTimeIdCommand(line, "less", time, id))
  {
    return false;
  }
  return executeDurationQuery(storage, id, 'l', time, output);
}

bool shaykhraziev::executeGreater(U2Storage& storage,
    const std::string& line,
    std::ostream& output)
{
  size_t time = 0;
  size_t id = 0;
  if (!parseTimeIdCommand(line, "greater", time, id))
  {
    return false;
  }
  return executeDurationQuery(storage, id, 'g', time, output);
}

bool shaykhraziev::executeCommons(U2Storage& storage,
    const std::string& line,
    std::ostream& output)
{
  size_t firstId = 0;
  size_t secondId = 0;
  if (!parseTwoIdCommand(line, "commons", firstId, secondId)
      || !contains(storage.knownIds, firstId)
      || !contains(storage.knownIds, secondId))
  {
    return false;
  }

  HashTable< size_t, bool > firstNeighbours;
  initHashTable(firstNeighbours, 101, hashSizeT, equalSizeT);
  List< size_t > result;
  initList(result);
  try
  {
    ListIterator< Meeting > iterator = begin(storage.meetings);
    while (!isEnd(iterator))
    {
      const Meeting& meeting = get(iterator);
      if (hasMeetingWith(meeting, firstId))
      {
        const bool known = true;
        insert(firstNeighbours, getOtherId(meeting, firstId), known);
      }
      iterator = next(iterator);
    }
    iterator = begin(storage.meetings);
    while (!isEnd(iterator))
    {
      const Meeting& meeting = get(iterator);
      if (hasMeetingWith(meeting, secondId))
      {
        const size_t other = getOtherId(meeting, secondId);
        if (contains(firstNeighbours, other))
        {
          insertOrderedUniqueSizeT(result, other);
        }
      }
      iterator = next(iterator);
    }
    printSizeTList(result, output);
  }
  catch (...)
  {
    clearList(result);
    clearHashTable(firstNeighbours);
    throw;
  }
  clearList(result);
  clearHashTable(firstNeighbours);
  return true;
}

bool shaykhraziev::executeDeanon(U2Storage& storage, const std::string& line)
{
  size_t anonId = 0;
  size_t id = 0;
  if (!parseTwoIdCommand(line, "deanon", anonId, id)
      || (anonId == id)
      || !contains(storage.knownIds, anonId)
      || (findPersonById(storage.personsById, anonId) != nullptr)
      || (findPersonById(storage.personsById, id) == nullptr))
  {
    return false;
  }

  ListIterator< Meeting > iterator = begin(storage.meetings);
  while (!isEnd(iterator))
  {
    Meeting& meeting = get(iterator);
    if (meeting.first == anonId)
    {
      meeting.first = id;
    }
    if (meeting.second == anonId)
    {
      meeting.second = id;
    }
    if (meeting.first == meeting.second)
    {
      iterator = erase(storage.meetings, iterator);
    }
    else
    {
      iterator = next(iterator);
    }
  }
  erase(storage.knownIds, anonId);
  return true;
}

bool shaykhraziev::executeOutPersons(U2Storage& storage,
    const std::string& line,
    std::ostream& result)
{
  std::string filename;
  if (!parseFileCommand(line, "out-persons", filename))
  {
    return false;
  }
  std::ofstream output(filename.c_str());
  if (!output)
  {
    return false;
  }
  writePersons(output, storage.persons);
  writePersons(result, storage.persons);
  return true;
}

bool shaykhraziev::executeCommand(U2Storage& storage,
    const std::string& line,
    std::ostream& output)
{
  if (parseExactCommand(line, "anons"))
  {
    return executeAnons(storage, output);
  }
  if (startsWith(line, "desc"))
  {
    return executeDesc(storage, line, output);
  }
  if (startsWith(line, "redesc"))
  {
    return executeRedesc(storage, line);
  }
  if (startsWith(line, "meet"))
  {
    return executeMeets(storage, line, output);
  }
  if (startsWith(line, "less"))
  {
    return executeLess(storage, line, output);
  }
  if (startsWith(line, "greater"))
  {
    return executeGreater(storage, line, output);
  }
  if (startsWith(line, "commons"))
  {
    return executeCommons(storage, line, output);
  }
  if (startsWith(line, "deanon"))
  {
    return executeDeanon(storage, line);
  }
  if (startsWith(line, "out-persons"))
  {
    return executeOutPersons(storage, line, output);
  }
  return false;
}

void shaykhraziev::runCommandLoop(U2Storage& storage, std::istream& input, std::ostream& output)
{
  std::string line;
  while (std::getline(input, line))
  {
    if (!executeCommand(storage, line, output))
    {
      output << "<INVALID COMMAND>\n";
    }
  }
}
