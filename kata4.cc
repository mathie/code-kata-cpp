// Code Kata 4: Data Munging
//
// $Id: kata4.cc,v 1.2 2004/02/02 15:06:25 mathie Exp $
//
// $Log: kata4.cc,v $
// Revision 1.2  2004/02/02 15:06:25  mathie
// * Implement the weather data parse and a test case to verify the result.
//
// Revision 1.1  2004/02/02 12:12:56  mathie
// Initial revision
//
//

// Problem definition:
// http://www.pragprog.com/pragdave/Practices/Kata/KataFour.rdoc

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test_framework.hpp>

#include <fstream>
#include <cstdlib>
#include <limits>

using boost::unit_test_framework::test_suite;
using namespace std;

unsigned int find_min_spread_day()
{
  ifstream f("K4Weather.txt");
  string line;
  int min_spread_day = 0;
  int min_spread = numeric_limits<int>::max();
  
  while(getline(f, line)) {
    const char whitespace[] = " \a\b\f\n\r\t\v";
    const char number[] = "0123456789.";
    
    // If the line has no non-whitespace characters or its first
    // non-whitespace character isn't a digit, ignore the line.
    size_t beg_field = line.find_first_not_of(whitespace);
    if(beg_field == string::npos || line.find_first_of(number) != beg_field) {
      continue;
    }
    
    size_t end_field = line.find_first_not_of(number, beg_field);
    int day = atoi(string(line, beg_field, end_field - beg_field).c_str());
    beg_field = line.find_first_of(number, end_field);
    end_field = line.find_first_not_of(number, beg_field);
    int day_max = atoi(string(line, beg_field, end_field - beg_field).c_str());
    beg_field = line.find_first_of(number, end_field);
    end_field = line.find_first_not_of(number, beg_field);
    int day_min = atoi(string(line, beg_field, end_field - beg_field).c_str());
    int day_spread = day_max - day_min;
    
    if(min_spread > (day_spread)) {
      min_spread_day = day;
      min_spread = day_spread;
    }
    
    // cout << day << ": " << day_max << " - " << day_min
    //      << " = " << day_spread
    //      << ((min_spread_day == day) ? " (new min!)" : "")
    //      << endl;
  }
  return min_spread_day;
}

void test_weather_data()
{
  // The goal of this task is to find the smallest range of temperature
  // in a single day.  This happens to be the 14th, which only had a
  // variation of 2 (discovered with a quick lump of shell:
  //   cat K4Weather.txt|awk '{print $1, $2 - $3}'
  // and a little hand-filtering).
  BOOST_CHECK(find_min_spread_day() == 14);
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
