// Code Kata 5: Bloom Filters
//
// $Id: kata5.cc,v 1.1 2004/02/02 18:03:57 mathie Exp $
//
// $Log: kata5.cc,v $
// Revision 1.1  2004/02/02 18:03:57  mathie
// Initial revision
//

// Problem definition:
// http://www.pragprog.com/pragdave/Practices/Kata/KataFive.rdoc

#include <boost/test/unit_test.hpp>

using boost::unit_test_framework::test_suite;

test_suite *init_unit_test_suite(int argc, char *argv[])
{
  test_suite *t = BOOST_TEST_SUITE("Code Kata 5: Bloom Filters");
  return t;
}
