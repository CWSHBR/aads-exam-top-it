#define BOOST_TEST_MODULE U1
#include <boost/test/included/unit_test.hpp>
#include "../common/list.hpp"

BOOST_AUTO_TEST_CASE(empty_list)
{
  shaykhraziev::List< int > list;
  shaykhraziev::initList(list);

  BOOST_TEST(list.head == nullptr);
  BOOST_TEST(list.tail == nullptr);
  BOOST_TEST(list.size == 0);
  BOOST_TEST(shaykhraziev::isEnd(shaykhraziev::begin(list)));
}

BOOST_AUTO_TEST_CASE(push_back_one_element)
{
  shaykhraziev::List< int > list;
  shaykhraziev::initList(list);

  int* value = shaykhraziev::pushBack(list, 42);

  BOOST_TEST(value != nullptr);
  BOOST_TEST(*value == 42);
  BOOST_TEST(list.size == 1);
  BOOST_TEST(list.head == list.tail);
  BOOST_TEST(shaykhraziev::get(shaykhraziev::begin(list)) == 42);

  shaykhraziev::clearList(list);
}

BOOST_AUTO_TEST_CASE(push_back_many_elements_and_iterate)
{
  shaykhraziev::List< int > list;
  shaykhraziev::initList(list);

  shaykhraziev::pushBack(list, 10);
  shaykhraziev::pushBack(list, 20);
  shaykhraziev::pushBack(list, 30);

  shaykhraziev::ListIterator< int > iterator = shaykhraziev::begin(list);
  BOOST_TEST(shaykhraziev::get(iterator) == 10);
  iterator = shaykhraziev::next(iterator);
  BOOST_TEST(shaykhraziev::get(iterator) == 20);
  iterator = shaykhraziev::next(iterator);
  BOOST_TEST(shaykhraziev::get(iterator) == 30);
  iterator = shaykhraziev::next(iterator);
  BOOST_TEST(shaykhraziev::isEnd(iterator));

  shaykhraziev::clearList(list);
}

BOOST_AUTO_TEST_CASE(erase_first_element)
{
  shaykhraziev::List< int > list;
  shaykhraziev::initList(list);
  shaykhraziev::pushBack(list, 1);
  shaykhraziev::pushBack(list, 2);
  shaykhraziev::pushBack(list, 3);

  shaykhraziev::ListIterator< int > iterator = shaykhraziev::erase(list, shaykhraziev::begin(list));

  BOOST_TEST(list.size == 2);
  BOOST_TEST(shaykhraziev::get(iterator) == 2);
  BOOST_TEST(shaykhraziev::get(shaykhraziev::begin(list)) == 2);

  shaykhraziev::clearList(list);
}

BOOST_AUTO_TEST_CASE(erase_last_element)
{
  shaykhraziev::List< int > list;
  shaykhraziev::initList(list);
  shaykhraziev::pushBack(list, 1);
  shaykhraziev::pushBack(list, 2);
  shaykhraziev::pushBack(list, 3);

  shaykhraziev::ListIterator< int > iterator = shaykhraziev::begin(list);
  iterator = shaykhraziev::next(iterator);
  iterator = shaykhraziev::next(iterator);
  iterator = shaykhraziev::erase(list, iterator);

  BOOST_TEST(list.size == 2);
  BOOST_TEST(shaykhraziev::isEnd(iterator));
  BOOST_TEST(list.tail->value == 2);

  shaykhraziev::clearList(list);
}

BOOST_AUTO_TEST_CASE(erase_middle_element)
{
  shaykhraziev::List< int > list;
  shaykhraziev::initList(list);
  shaykhraziev::pushBack(list, 1);
  shaykhraziev::pushBack(list, 2);
  shaykhraziev::pushBack(list, 3);

  shaykhraziev::ListIterator< int > iterator = shaykhraziev::next(shaykhraziev::begin(list));
  iterator = shaykhraziev::erase(list, iterator);

  BOOST_TEST(list.size == 2);
  BOOST_TEST(shaykhraziev::get(iterator) == 3);
  BOOST_TEST(list.head->next == list.tail);
  BOOST_TEST(list.tail->prev == list.head);

  shaykhraziev::clearList(list);
}

BOOST_AUTO_TEST_CASE(pop_back_element)
{
  shaykhraziev::List< int > list;
  shaykhraziev::initList(list);
  shaykhraziev::pushBack(list, 1);
  shaykhraziev::pushBack(list, 2);

  BOOST_TEST(shaykhraziev::popBack(list));
  BOOST_TEST(list.size == 1);
  BOOST_TEST(list.tail->value == 1);
  BOOST_TEST(shaykhraziev::popBack(list));
  BOOST_TEST(!shaykhraziev::popBack(list));
  BOOST_TEST(list.size == 0);

  shaykhraziev::clearList(list);
}

BOOST_AUTO_TEST_CASE(clear_list)
{
  shaykhraziev::List< int > list;
  shaykhraziev::initList(list);
  shaykhraziev::pushBack(list, 1);
  shaykhraziev::pushBack(list, 2);

  shaykhraziev::clearList(list);

  BOOST_TEST(list.head == nullptr);
  BOOST_TEST(list.tail == nullptr);
  BOOST_TEST(list.size == 0);
}
