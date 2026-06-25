#ifndef ORDERED_LIST_HPP
#define ORDERED_LIST_HPP

#include "../common/domain.hpp"
#include "../common/list.hpp"

namespace shaykhraziev
{
  inline bool isLessMeetView(const MeetView& left, const MeetView& right)
  {
    return (left.id < right.id) || ((left.id == right.id) && (left.duration < right.duration));
  }

  inline bool isLessDurationView(const MeetView& left, const MeetView& right)
  {
    return (left.duration < right.duration)
        || ((left.duration == right.duration) && (left.id < right.id));
  }

  template< class T >
  T* insertBefore(List< T >& list, ListIterator< T > iterator, const T& value)
  {
    if (iterator.node == nullptr)
    {
      return pushBack(list, value);
    }
    ListNode< T >* nextNode = iterator.node;
    ListNode< T >* node = new ListNode< T >{ value, nextNode, nextNode->prev };
    if (nextNode->prev != nullptr)
    {
      nextNode->prev->next = node;
    }
    else
    {
      list.head = node;
    }
    nextNode->prev = node;
    ++list.size;
    return &node->value;
  }

  inline bool insertOrderedUniqueSizeT(List< size_t >& values, size_t value)
  {
    ListIterator< size_t > iterator = begin(values);
    while (!isEnd(iterator))
    {
      if (get(iterator) == value)
      {
        return false;
      }
      if (value < get(iterator))
      {
        insertBefore(values, iterator, value);
        return true;
      }
      iterator = next(iterator);
    }
    pushBack(values, value);
    return true;
  }

  inline MeetView* insertOrderedMeetView(List< MeetView >& values, const MeetView& value)
  {
    ListIterator< MeetView > iterator = begin(values);
    while (!isEnd(iterator))
    {
      if (isLessMeetView(value, get(iterator)))
      {
        return insertBefore(values, iterator, value);
      }
      iterator = next(iterator);
    }
    return pushBack(values, value);
  }

  inline MeetView* insertOrderedDurationView(List< MeetView >& values, const MeetView& value)
  {
    ListIterator< MeetView > iterator = begin(values);
    while (!isEnd(iterator))
    {
      if (isLessDurationView(value, get(iterator)))
      {
        return insertBefore(values, iterator, value);
      }
      iterator = next(iterator);
    }
    return pushBack(values, value);
  }
}

#endif
