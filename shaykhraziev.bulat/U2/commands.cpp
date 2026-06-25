#include "commands.hpp"
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

  bool parseRedescCommand(const std::string& line, size_t& id, std::string& description)
  {
    size_t position = 0;
    if (!parseCommandPrefix(line, "redesc", position))
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

  void printMeetViews(shaykhraziev::List< shaykhraziev::MeetView >& views, std::ostream& output)
  {
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
}

bool shaykhraziev::executeAnons(U2Storage& storage, std::ostream& output)
{
  List< size_t > anons;
  initList(anons);
  AnonsData data = { &storage, &anons };
  forEachEntry(storage.knownIds, collectAnonId, &data);
  ListIterator< size_t > iterator = begin(anons);
  while (!isEnd(iterator))
  {
    output << get(iterator) << '\n';
    iterator = next(iterator);
  }
  clearList(anons);
  return true;
}

bool shaykhraziev::executeDesc(U2Storage& storage, const std::string& line, std::ostream& output)
{
  size_t id = 0;
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
  if (!parseRedescCommand(line, id, description) || !contains(storage.knownIds, id))
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
  if (!parseIdCommand(line, "meets", id))
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
  return executeMeetQuery(storage, id, 'l', time, output);
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
  return executeMeetQuery(storage, id, 'g', time, output);
}
