// Code Kata 2: Binary chop
//
// $Id: kata2.cc,v 1.1 2004/02/02 09:54:56 mathie Exp $
//
// $Log: kata2.cc,v $
// Revision 1.1  2004/02/02 09:54:56  mathie
// Initial revision
//

// Problem definition:
// http://www.pragprog.com/pragdave/Practices/Kata/KataTwo.rdoc
//
// Each implementation of the binary chop algorithm takes the form:
//   chop(int val, iterator begin, iterator end) -> iterator
// where val is the integer to search for, begin is an iterator pointing
// to the start of the list, end is an iterator pointing to
// one-past-the-end and the returned value is an iterator pointing to
// the found item (or end if it's not found).  This is a slight
// C++ification of the functor definition in the problem, but seems
// like a good idea to me...

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test_framework.hpp>

using boost::unit_test_framework::test_suite;
using namespace std;

// Monday is the recursive case
const int *monday(const int val, const int *begin, const int *end)
{
  // Failure case
  if (begin == end) {
    return end;
  }
  
  const int *middle = begin + (end - begin) / 2;
  if (val == *middle) {
    // Success!
    return middle;
  } else if (val > *middle) {
    // Somewhere in the left of centre.
    return monday(val, middle + 1, end);
  } else {
    // Somewhere to the right of centre.  This is where it gets slightly
    // icky -- we have to explicitly test for failure here because
    // 'middle' is not one-past-the-end of the calling function it's,
    // well, slap bang in the middle.
    const int *result = monday(val, begin, middle);
    if (result == middle) {
      return end;
    }
    return result;
  }
}

void test_monday()
{
  const int t1[] = {};
  const size_t t1_size = sizeof(t1) / sizeof(t1[0]);
  const int *t1_end = t1 + t1_size;
  BOOST_CHECK(t1_end == monday(3, t1, t1_end));

  const int t2[] = { 1 };
  const size_t t2_size = sizeof(t2) / sizeof(t2[0]);
  const int *t2_end = t2 + t2_size;
  BOOST_CHECK(t2_end == monday(3, t2, t2_end));
  BOOST_CHECK(t2[0] == *monday(1, t2, t2_end));

  const int t3[] = { 1, 3, 5 };
  const size_t t3_size = sizeof(t3) / sizeof(t3[0]);
  const int *t3_end = t3 + t3_size;
  BOOST_CHECK(t3[0] == *monday(1, t3, t3_end));
  BOOST_CHECK(t3[1] == *monday(3, t3, t3_end));
  BOOST_CHECK(t3[2] == *monday(5, t3, t3_end));
  BOOST_CHECK(t3_end == monday(0, t3, t3_end));
  BOOST_CHECK(t3_end == monday(2, t3, t3_end));
  BOOST_CHECK(t3_end == monday(4, t3, t3_end));
  BOOST_CHECK(t3_end == monday(6, t3, t3_end));

  const int t4[] = { 1, 3, 5, 7 };
  const size_t t4_size = sizeof(t4) / sizeof(t4[0]);
  const int *t4_end = t4 + t4_size;
  BOOST_CHECK(t4[0] == *monday(1, t4, t4_end));
  BOOST_CHECK(t4[1] == *monday(3, t4, t4_end));
  BOOST_CHECK(t4[2] == *monday(5, t4, t4_end));
  BOOST_CHECK(t4[3] == *monday(7, t4, t4_end));
  BOOST_CHECK(t4_end == monday(0, t4, t4_end));
  BOOST_CHECK(t4_end == monday(2, t4, t4_end));
  BOOST_CHECK(t4_end == monday(4, t4, t4_end));
  BOOST_CHECK(t4_end == monday(6, t4, t4_end));
  BOOST_CHECK(t4_end == monday(8, t4, t4_end));
}

test_suite *init_unit_test_suite(int argc, char *argv[])
{
  test_suite *t = BOOST_TEST_SUITE("Code Kata 2: Binary Chop");
  t->add(BOOST_TEST_CASE(&test_monday));
  return t;
}
