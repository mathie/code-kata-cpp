// Code Kata 6: Anagrams
//
// $Id: kata6.cc,v 1.1 2004/02/05 16:37:36 mathie Exp $
//
// $Log: kata6.cc,v $
// Revision 1.1  2004/02/05 16:37:36  mathie
// Initial revision
//

// Problem definition:
// http://www.pragprog.com/pragdave/Practices/Kata/KataSix.rdoc

#include <boost/test/unit_test.hpp>

using boost::unit_test_framework::test_suite;
using namespace std;

test_suite *init_unit_test_suite(int argc, char *argv[])
{
  test_suite *t = BOOST_TEST_SUITE("Code Kata 6: Anagrams");

  return t;
}
