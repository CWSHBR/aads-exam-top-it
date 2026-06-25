#include <boost/test/unit_test.hpp>
#include "../common/ordered-list.hpp"

BOOST_AUTO_TEST_CASE(insert_ordered_unique_size_t_values)
{
  shaykhraziev::List< size_t > values;
  shaykhraziev::initList(values);

  BOOST_TEST(shaykhraziev::insertOrderedUniqueSizeT(values, 30));
  BOOST_TEST(shaykhraziev::insertOrderedUniqueSizeT(values, 10));
  BOOST_TEST(shaykhraziev::insertOrderedUniqueSizeT(values, 20));
  BOOST_TEST(!shaykhraziev::insertOrderedUniqueSizeT(values, 20));

  shaykhraziev::ListIterator< size_t > iterator = shaykhraziev::begin(values);
  BOOST_TEST(shaykhraziev::get(iterator) == 10);
  iterator = shaykhraziev::next(iterator);
  BOOST_TEST(shaykhraziev::get(iterator) == 20);
  iterator = shaykhraziev::next(iterator);
  BOOST_TEST(shaykhraziev::get(iterator) == 30);
  iterator = shaykhraziev::next(iterator);
  BOOST_TEST(shaykhraziev::isEnd(iterator));

  shaykhraziev::clearList(values);
}

BOOST_AUTO_TEST_CASE(insert_ordered_meet_views)
{
  shaykhraziev::List< shaykhraziev::MeetView > values;
  shaykhraziev::initList(values);

  shaykhraziev::insertOrderedMeetView(values, shaykhraziev::MeetView{ 33, 99 });
  shaykhraziev::insertOrderedMeetView(values, shaykhraziev::MeetView{ 32, 11 });
  shaykhraziev::insertOrderedMeetView(values, shaykhraziev::MeetView{ 33, 10 });

  shaykhraziev::ListIterator< shaykhraziev::MeetView > iterator = shaykhraziev::begin(values);
  BOOST_TEST(shaykhraziev::get(iterator).id == 32);
  BOOST_TEST(shaykhraziev::get(iterator).duration == 11);
  iterator = shaykhraziev::next(iterator);
  BOOST_TEST(shaykhraziev::get(iterator).id == 33);
  BOOST_TEST(shaykhraziev::get(iterator).duration == 10);
  iterator = shaykhraziev::next(iterator);
  BOOST_TEST(shaykhraziev::get(iterator).id == 33);
  BOOST_TEST(shaykhraziev::get(iterator).duration == 99);

  shaykhraziev::clearList(values);
}
