// Code Kata 5: Bloom Filters
//
// $Id: kata5.cc,v 1.2 2004/02/02 19:57:23 mathie Exp $
//
// $Log: kata5.cc,v $
// Revision 1.2  2004/02/02 19:57:23  mathie
// * Implement a (very) simple templated bloom filter, with a user-defined
//   function to produce a list of hashes and the map size.
// * Implement a simple hashing functor which will produce a 'hash' based
//   directly upon each individual character.
// * A test case showing insertions, successful lookups and a false
// * positive.
//
// Revision 1.1  2004/02/02 18:03:57  mathie
// Initial revision
//

// Problem definition:
// http://www.pragprog.com/pragdave/Practices/Kata/KataFive.rdoc

#include <boost/test/unit_test.hpp>

#include <string>
#include <bitset>
#include <vector>

using boost::unit_test_framework::test_suite;
using namespace std;

typedef vector<int> hash_list;

template <class hash_fn, int map_size = hash_fn::map_size>
class bloom_filter 
{
  bitset<map_size> map;
  hash_fn hashes;
  
 public:

  void insert(const string& word)
  {
    hash_list h = hashes(word);
    for(hash_list::const_iterator i = h.begin(); i != h.end(); i++) {
      map.set(*i);
    }
  }
  
  bool lookup(const string& word) const
  {
    hash_list h = hashes(word);
    for(hash_list::const_iterator i = h.begin(); i != h.end(); i++) {
      if(!map[*i]) {
        return false;
      }
    }
    return true;
  }
  
};

class split_into_chars
{
 public:
  static const int map_size;
  
  hash_list operator()(const string& word) const
  {
    hash_list h;
    for(string::const_iterator i = word.begin(); i != word.end(); i++) {
      h.push_back(static_cast<int>(*i));
    }
    return h;
  }
};

const int split_into_chars::map_size = 256;

void test_split_into_chars_hash()
{
  bloom_filter<split_into_chars> bf;
  bf.insert("foo");
  bf.insert("bar");
  bf.insert("bazification");

  // Successful lookups
  BOOST_CHECK(bf.lookup("foo") == true);
  BOOST_CHECK(bf.lookup("bar") == true);
  BOOST_CHECK(bf.lookup("bazification") == true);

  // Try some things not in the dictionary
  BOOST_CHECK(bf.lookup("notindict") == false);

  // A false positive
  BOOST_CHECK(bf.lookup("roof") == true);
  
}

test_suite *init_unit_test_suite(int argc, char *argv[])
{
  test_suite *t = BOOST_TEST_SUITE("Code Kata 5: Bloom Filters");
  t->add(BOOST_TEST_CASE(&test_split_into_chars_hash));
  return t;
}
