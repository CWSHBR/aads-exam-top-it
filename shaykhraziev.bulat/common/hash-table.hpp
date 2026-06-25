#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <cstddef>
#include "../common/list.hpp"

namespace shaykhraziev
{
  template< class Key, class Value >
  struct HashEntry
  {
    Key key;
    Value value;
  };

  template< class Key, class Value >
  struct HashTable
  {
    List< HashEntry< Key, Value > >* buckets;
    size_t bucketCount;
    size_t size;
    size_t (*hash)(const Key&);
    bool (*equal)(const Key&, const Key&);
  };

  template< class Key, class Value >
  void initHashTable(HashTable< Key, Value >& table,
      size_t bucketCount,
      size_t (*hash)(const Key&),
      bool (*equal)(const Key&, const Key&))
  {
    table.buckets = nullptr;
    table.bucketCount = 0;
    table.size = 0;
    table.hash = hash;
    table.equal = equal;
    if (bucketCount == 0)
    {
      return;
    }
    List< HashEntry< Key, Value > >* buckets = new List< HashEntry< Key, Value > >[bucketCount];
    for (size_t i = 0; i < bucketCount; ++i)
    {
      initList(buckets[i]);
    }
    table.buckets = buckets;
    table.bucketCount = bucketCount;
  }

  template< class Key, class Value >
  void clearHashTable(HashTable< Key, Value >& table)
  {
    if (table.buckets != nullptr)
    {
      for (size_t i = 0; i < table.bucketCount; ++i)
      {
        clearList(table.buckets[i]);
      }
      delete[] table.buckets;
    }
    table.buckets = nullptr;
    table.bucketCount = 0;
    table.size = 0;
  }

  template< class Key, class Value >
  Value* find(HashTable< Key, Value >& table, const Key& key)
  {
    if ((table.buckets == nullptr) || (table.bucketCount == 0))
    {
      return nullptr;
    }
    const size_t index = table.hash(key) % table.bucketCount;
    ListIterator< HashEntry< Key, Value > > iterator = begin(table.buckets[index]);
    while (!isEnd(iterator))
    {
      HashEntry< Key, Value >& entry = get(iterator);
      if (table.equal(entry.key, key))
      {
        return &entry.value;
      }
      iterator = next(iterator);
    }
    return nullptr;
  }

  template< class Key, class Value >
  const Value* findConst(const HashTable< Key, Value >& table, const Key& key)
  {
    if ((table.buckets == nullptr) || (table.bucketCount == 0))
    {
      return nullptr;
    }
    const size_t index = table.hash(key) % table.bucketCount;
    ListIterator< HashEntry< Key, Value > > iterator;
    iterator.node = table.buckets[index].head;
    while (!isEnd(iterator))
    {
      const HashEntry< Key, Value >& entry = getConst(iterator);
      if (table.equal(entry.key, key))
      {
        return &entry.value;
      }
      iterator = next(iterator);
    }
    return nullptr;
  }

  template< class Key, class Value >
  bool contains(HashTable< Key, Value >& table, const Key& key)
  {
    return find(table, key) != nullptr;
  }

  template< class Key, class Value >
  bool insert(HashTable< Key, Value >& table, const Key& key, const Value& value)
  {
    if ((table.buckets == nullptr) || (table.bucketCount == 0) || (find(table, key) != nullptr))
    {
      return false;
    }
    const size_t index = table.hash(key) % table.bucketCount;
    const HashEntry< Key, Value > entry = { key, value };
    pushBack(table.buckets[index], entry);
    ++table.size;
    return true;
  }

  template< class Key, class Value >
  bool erase(HashTable< Key, Value >& table, const Key& key)
  {
    if ((table.buckets == nullptr) || (table.bucketCount == 0))
    {
      return false;
    }
    const size_t index = table.hash(key) % table.bucketCount;
    ListIterator< HashEntry< Key, Value > > iterator = begin(table.buckets[index]);
    while (!isEnd(iterator))
    {
      if (table.equal(get(iterator).key, key))
      {
        erase(table.buckets[index], iterator);
        --table.size;
        return true;
      }
      iterator = next(iterator);
    }
    return false;
  }

  template< class Key, class Value >
  void swapHashTables(HashTable< Key, Value >& left, HashTable< Key, Value >& right)
  {
    List< HashEntry< Key, Value > >* buckets = left.buckets;
    const size_t bucketCount = left.bucketCount;
    const size_t size = left.size;
    size_t (*hash)(const Key&) = left.hash;
    bool (*equal)(const Key&, const Key&) = left.equal;

    left.buckets = right.buckets;
    left.bucketCount = right.bucketCount;
    left.size = right.size;
    left.hash = right.hash;
    left.equal = right.equal;

    right.buckets = buckets;
    right.bucketCount = bucketCount;
    right.size = size;
    right.hash = hash;
    right.equal = equal;
  }

  template< class Key, class Value >
  void rehash(HashTable< Key, Value >& table, size_t bucketCount)
  {
    HashTable< Key, Value > newTable;
    initHashTable(newTable, bucketCount, table.hash, table.equal);
    try
    {
      for (size_t i = 0; i < table.bucketCount; ++i)
      {
        ListIterator< HashEntry< Key, Value > > iterator = begin(table.buckets[i]);
        while (!isEnd(iterator))
        {
          const HashEntry< Key, Value >& entry = getConst(iterator);
          insert(newTable, entry.key, entry.value);
          iterator = next(iterator);
        }
      }
    }
    catch (...)
    {
      clearHashTable(newTable);
      throw;
    }
    swapHashTables(table, newTable);
    clearHashTable(newTable);
  }

  template< class Key, class Value >
  void forEachEntry(HashTable< Key, Value >& table, void (*function)(HashEntry< Key, Value >&, void*),
      void* data)
  {
    for (size_t i = 0; i < table.bucketCount; ++i)
    {
      ListIterator< HashEntry< Key, Value > > iterator = begin(table.buckets[i]);
      while (!isEnd(iterator))
      {
        function(get(iterator), data);
        iterator = next(iterator);
      }
    }
  }
}

#endif
