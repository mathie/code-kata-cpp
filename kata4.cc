// Code Kata 4: Data Munging
//
// $Id: kata4.cc,v 1.4 2004/02/02 17:13:16 mathie Exp $
//
// $Log: kata4.cc,v $
// Revision 1.4  2004/02/02 17:13:16  mathie
// * Implement new versions of the functions from parts one and two,
//   factoring out as much common code as possible.  Personally, I think
//   I've found a rather elegant way of consolidating the code. :-)
//
// Revision 1.3  2004/02/02 16:37:11  mathie
// * Implement the soccer table parser and a test case to verify the
// * result.
//
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
#include <string>
#include <vector>

using boost::unit_test_framework::test_suite;
using namespace std;

const char whitespace[] = " \a\b\f\n\r\t\v";
const char number[] = "0123456789.";
const char alpha[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "abcdefghijklmnopqrstuvwxyz";

unsigned int find_min_spread_day()
{
  ifstream f("K4Weather.txt");
  string line;
  int min_spread_day = 0;
  int min_spread = numeric_limits<int>::max();
  
  while(getline(f, line)) {
    
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

string find_min_goal_diff()
{
  ifstream f("K4Soccer.txt");
  string line, min_team_name;
  int min_diff = numeric_limits<int>::max();

  while(getline(f, line)) {
    // If the line has no non-whitespace characters or its first
    // non-whitespace character isn't a digit, ignore the line.
    size_t beg_field = line.find_first_not_of(whitespace);
    if(beg_field == string::npos || line.find_first_of(number) != beg_field) {
      continue;
    }

    beg_field = line.find_first_of(alpha);
    size_t end_field = line.find_last_of(alpha,
                                         line.find_first_of(number,
                                                            beg_field));
    string team_name(line, beg_field, end_field - beg_field + 1);
    for(int i = 0; i < 5; i++) {
      beg_field = line.find_first_of(number, end_field);
      end_field = line.find_first_not_of(number, beg_field);
    }
    int team_for = atoi(string(line, beg_field,
                               end_field - beg_field).c_str());
    beg_field = line.find_first_of(number, end_field);
    end_field = line.find_first_not_of(number, beg_field);
    int team_against = atoi(string(line, beg_field,
                                   end_field - beg_field).c_str());
    int team_diff = abs(team_for - team_against);
    if(min_diff > team_diff) {
      min_diff = team_diff;
      min_team_name = team_name;
    }

    //cout << team_name << ": " << team_for << " - " << team_against
    //     << " = " << team_diff
    //     << ((min_team_name == team_name) ? " (new min!)" : "")
    //     << endl;
  }
  return min_team_name;
}

typedef vector<string> row;
typedef vector<row> table;

// Parse a file into a vector of vectors, each containing strings
// corresponding to the individual cells.
table parse_table(const string& file) 
{
  table t;
  ifstream f(file.c_str());
  string line;
  
  while(getline(f, line)) {
    // If the line has no non-whitespace characters or its first
    // non-whitespace character isn't a digit, ignore the line.
    size_t beg_field = line.find_first_not_of(whitespace);
    if(beg_field == string::npos || line.find_first_of(number) != beg_field) {
      continue;
    }
    
    row r;
    size_t end_field;
    
    while(beg_field != string::npos) {
      end_field = line.find_first_of(whitespace, beg_field);
      if (end_field != string::npos) {
        r.push_back(string(line, beg_field, end_field - beg_field));
      } else {
        r.push_back(string(line, beg_field));
        break;
      }
      beg_field = line.find_first_not_of(whitespace, end_field);
    }
    t.push_back(r);
  }
  return t;
}

// Given a table returned from parse_table(), along with the
// zero-indexed result column and two columns to take the difference
// between, return the intersection of the result column and the row
// which contains the minimum difference between the two other columns.
// Simpler to code than explain, apparently. :-)
string find_min_diff(table& t, int res_col, int col_a, int col_b)
{
  string cur_min_str;
  int cur_min_val = numeric_limits<int>::max();
  
  for(table::const_iterator row = t.begin(); row != t.end(); row++) {
    int a = atoi((*row)[col_a].c_str());
    int b = atoi((*row)[col_b].c_str());
    int diff = abs(a - b);
    if(diff < cur_min_val) {
      cur_min_val = diff;
      cur_min_str = (*row)[res_col];
    }
  }
  return cur_min_str;
}

unsigned int find_min_spread_day_common()
{
  table t = parse_table("K4Weather.txt");
  return atoi(find_min_diff(t, 0, 1, 2).c_str());
}

string find_min_goal_diff_common()
{
  table t = parse_table("K4Soccer.txt");
  return find_min_diff(t, 1, 6, 8);
}

void test_weather_data()
{
  // The goal of this task is to find the smallest range of temperature
  // in a single day.  This happens to be the 14th, which only had a
  // variation of 2 (discovered with a quick lump of shell:
  //   cat K4Weather.txt|awk '{print $1, $2 - $3}'
  // and a little hand-filtering).
  BOOST_CHECK(find_min_spread_day() == 14);
  BOOST_CHECK(find_min_spread_day_common() == 14);
}

void test_soccer_league_table()
{
  // The goal of this task is to parse K4Soccer.txt and find the minimum
  // goal difference in the 2001-2002 premier league.  This happens to
  // be Aston Villa, with a goal difference of only 1 (discovered with
  // the awk script:
  //    cat K4Soccer.txt | awk 'function abs(a) {if (a<0) a=-a; return a;}
  //    { diff = abs($7-$9); if(diff>0)print diff, $2 }'
  // and manual scanning).
  //
  // Do not ask what that odd whitespace character is all about...
  BOOST_CHECK(find_min_goal_diff() == "Aston\x5fVilla");
  BOOST_CHECK(find_min_goal_diff_common() == "Aston\x5fVilla");
}

test_suite *init_unit_test_suite(int argc, char *argv[])
{
  test_suite *t = BOOST_TEST_SUITE("Code Kata 4: Data Munging");
  t->add(BOOST_TEST_CASE(&test_weather_data));
  t->add(BOOST_TEST_CASE(&test_soccer_league_table));
  return t;
}
