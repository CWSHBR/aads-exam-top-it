#include <boost/test/unit_test.hpp>
#include "../common/hash-table.hpp"

namespace
{
  size_t hashInt(const int& value)
  {
    return static_cast< size_t >(value);
  }

  size_t hashIntCollision(const int&)
  {
    return 0;
  }

  bool equalInt(const int& left, const int& right)
  {
    return left == right;
  }

  void countEntry(shaykhraziev::HashEntry< int, int >& entry, void* data)
  {
    size_t* count = static_cast< size_t* >(data);
    *count += 1;
    entry.value += 1;
  }
}

BOOST_AUTO_TEST_CASE(empty_hash_table)
{
  shaykhraziev::HashTable< int, int > table;
  shaykhraziev::initHashTable(table, 5, hashInt, equalInt);

  BOOST_TEST(table.size == 0);
  BOOST_TEST(shaykhraziev::find(table, 1) == nullptr);
  BOOST_TEST(!shaykhraziev::contains(table, 1));

  shaykhraziev::clearHashTable(table);
}

BOOST_AUTO_TEST_CASE(insert_and_find_hash_table_value)
{
  shaykhraziev::HashTable< int, int > table;
  shaykhraziev::initHashTable(table, 5, hashInt, equalInt);

  BOOST_TEST(shaykhraziev::insert(table, 1, 10));
  int* value = shaykhraziev::find(table, 1);

  BOOST_TEST(value != nullptr);
  BOOST_TEST(*value == 10);
  BOOST_TEST(table.size == 1);

  shaykhraziev::clearHashTable(table);
}

BOOST_AUTO_TEST_CASE(reject_duplicate_key)
{
  shaykhraziev::HashTable< int, int > table;
  shaykhraziev::initHashTable(table, 5, hashInt, equalInt);

  BOOST_TEST(shaykhraziev::insert(table, 1, 10));
  BOOST_TEST(!shaykhraziev::insert(table, 1, 20));

  int* value = shaykhraziev::find(table, 1);
  BOOST_TEST(value != nullptr);
  BOOST_TEST(*value == 10);
  BOOST_TEST(table.size == 1);

  shaykhraziev::clearHashTable(table);
}

BOOST_AUTO_TEST_CASE(erase_hash_table_value)
{
  shaykhraziev::HashTable< int, int > table;
  shaykhraziev::initHashTable(table, 5, hashInt, equalInt);
  shaykhraziev::insert(table, 1, 10);
  shaykhraziev::insert(table, 2, 20);

  BOOST_TEST(shaykhraziev::erase(table, 1));
  BOOST_TEST(shaykhraziev::find(table, 1) == nullptr);
  BOOST_TEST(shaykhraziev::find(table, 2) != nullptr);
  BOOST_TEST(table.size == 1);

  shaykhraziev::clearHashTable(table);
}

BOOST_AUTO_TEST_CASE(erase_absent_hash_table_value)
{
  shaykhraziev::HashTable< int, int > table;
  shaykhraziev::initHashTable(table, 5, hashInt, equalInt);

  BOOST_TEST(!shaykhraziev::erase(table, 1));
  BOOST_TEST(table.size == 0);

  shaykhraziev::clearHashTable(table);
}

BOOST_AUTO_TEST_CASE(handle_many_collisions)
{
  shaykhraziev::HashTable< int, int > table;
  shaykhraziev::initHashTable(table, 3, hashIntCollision, equalInt);

  BOOST_TEST(shaykhraziev::insert(table, 1, 10));
  BOOST_TEST(shaykhraziev::insert(table, 2, 20));
  BOOST_TEST(shaykhraziev::insert(table, 3, 30));

  BOOST_TEST(*shaykhraziev::find(table, 1) == 10);
  BOOST_TEST(*shaykhraziev::find(table, 2) == 20);
  BOOST_TEST(*shaykhraziev::find(table, 3) == 30);

  shaykhraziev::clearHashTable(table);
}

BOOST_AUTO_TEST_CASE(for_each_entry_visits_values)
{
  shaykhraziev::HashTable< int, int > table;
  shaykhraziev::initHashTable(table, 5, hashInt, equalInt);
  shaykhraziev::insert(table, 1, 10);
  shaykhraziev::insert(table, 2, 20);
  size_t count = 0;

  shaykhraziev::forEachEntry(table, countEntry, &count);

  BOOST_TEST(count == 2);
  BOOST_TEST(*shaykhraziev::find(table, 1) == 11);
  BOOST_TEST(*shaykhraziev::find(table, 2) == 21);

  shaykhraziev::clearHashTable(table);
}

BOOST_AUTO_TEST_CASE(rehash_preserves_values)
{
  shaykhraziev::HashTable< int, int > table;
  shaykhraziev::initHashTable(table, 2, hashInt, equalInt);
  shaykhraziev::insert(table, 1, 10);
  shaykhraziev::insert(table, 2, 20);
  shaykhraziev::insert(table, 3, 30);

  shaykhraziev::rehash(table, 7);

  BOOST_TEST(table.bucketCount == 7);
  BOOST_TEST(table.size == 3);
  BOOST_TEST(*shaykhraziev::find(table, 1) == 10);
  BOOST_TEST(*shaykhraziev::find(table, 2) == 20);
  BOOST_TEST(*shaykhraziev::find(table, 3) == 30);

  shaykhraziev::clearHashTable(table);
}

BOOST_AUTO_TEST_CASE(clear_hash_table)
{
  shaykhraziev::HashTable< int, int > table;
  shaykhraziev::initHashTable(table, 5, hashInt, equalInt);
  shaykhraziev::insert(table, 1, 10);

  shaykhraziev::clearHashTable(table);

  BOOST_TEST(table.buckets == nullptr);
  BOOST_TEST(table.bucketCount == 0);
  BOOST_TEST(table.size == 0);
}
