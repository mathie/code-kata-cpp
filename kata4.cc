// Code Kata 4: Data Munging
//
// $Id: kata4.cc,v 1.1 2004/02/02 12:12:56 mathie Exp $
//
// $Log: kata4.cc,v $
// Revision 1.1  2004/02/02 12:12:56  mathie
// Initial revision
//
//

// Problem definition:
// http://www.pragprog.com/pragdave/Practices/Kata/KataFour.rdoc

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test_framework.hpp>

using boost::unit_test_framework::test_suite;
using namespace std;

void test_weather_data()
{
}

void test_soccer_league_table()
{
}

test_suite *init_unit_test_suite(int argc, char *argv[])
{
  test_suite *t = BOOST_TEST_SUITE("Code Kata 4: Data Munging");
  t->add(BOOST_TEST_CASE(&test_weather_data));
  t->add(BOOST_TEST_CASE(&test_soccer_league_table));
  return t;
}
