#ifndef LIST_HPP
#define LIST_HPP

#include <cstddef>

namespace shaykhraziev
{
  template< class T >
  struct ListNode
  {
    T value;
    ListNode< T >* next;
    ListNode< T >* prev;
  };

  template< class T >
  struct List
  {
    ListNode< T >* head;
    ListNode< T >* tail;
    size_t size;
  };

  template< class T >
  struct ListIterator
  {
    ListNode< T >* node;
  };

  template< class T >
  void initList(List< T >& list)
  {
    list.head = nullptr;
    list.tail = nullptr;
    list.size = 0;
  }

  template< class T >
  void clearList(List< T >& list)
  {
    ListNode< T >* current = list.head;
    while (current != nullptr)
    {
      ListNode< T >* nextNode = current->next;
      delete current;
      current = nextNode;
    }
    initList(list);
  }

  template< class T >
  T* pushBack(List< T >& list, const T& value)
  {
    ListNode< T >* node = new ListNode< T >{ value, nullptr, list.tail };
    if (list.tail != nullptr)
    {
      list.tail->next = node;
    }
    else
    {
      list.head = node;
    }
    list.tail = node;
    ++list.size;
    return &node->value;
  }

  template< class T >
  ListIterator< T > begin(List< T >& list)
  {
    return ListIterator< T >{ list.head };
  }

  template< class T >
  ListIterator< T > end(List< T >&)
  {
    return ListIterator< T >{ nullptr };
  }

  template< class T >
  ListIterator< T > next(ListIterator< T > iterator)
  {
    if (iterator.node != nullptr)
    {
      iterator.node = iterator.node->next;
    }
    return iterator;
  }

  template< class T >
  ListIterator< T > prev(ListIterator< T > iterator)
  {
    if (iterator.node != nullptr)
    {
      iterator.node = iterator.node->prev;
    }
    return iterator;
  }

  template< class T >
  bool isEnd(ListIterator< T > iterator)
  {
    return iterator.node == nullptr;
  }

  template< class T >
  T& get(ListIterator< T > iterator)
  {
    return iterator.node->value;
  }

  template< class T >
  ListIterator< T > erase(List< T >& list, ListIterator< T > iterator)
  {
    if (iterator.node == nullptr)
    {
      return iterator;
    }
    ListNode< T >* node = iterator.node;
    ListNode< T >* nextNode = node->next;
    if (node->prev != nullptr)
    {
      node->prev->next = node->next;
    }
    else
    {
      list.head = node->next;
    }
    if (node->next != nullptr)
    {
      node->next->prev = node->prev;
    }
    else
    {
      list.tail = node->prev;
    }
    delete node;
    --list.size;
    return ListIterator< T >{ nextNode };
  }

  template< class T >
  bool popBack(List< T >& list)
  {
    if (list.tail == nullptr)
    {
      return false;
    }
    erase(list, ListIterator< T >{ list.tail });
    return true;
  }
}

#endif
