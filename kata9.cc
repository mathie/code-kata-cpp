// Code Kata 9: Back to the Checkout
//
// $Id: kata9.cc,v 1.1 2004/02/09 11:54:26 mathie Exp $
//
// $Log: kata9.cc,v $
// Revision 1.1  2004/02/09 11:54:26  mathie
// Initial revision
//

// Problem definition:
// http://www.pragprog.com/pragdave/Practices/Kata/KataNine.rdoc

#include <boost/test/unit_test.hpp>

using boost::unit_test_framework::test_suite;
using namespace std;

test_suite *init_unit_test_suite(int argc, char *argv[])
{
  test_suite *t = BOOST_TEST_SUITE("Code Kata 9: Back to the Checkout");
  return t;
}
