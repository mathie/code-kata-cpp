// Code Kata 5: Bloom Filters
//
// $Id: kata5.cc,v 1.3 2004/02/03 07:50:00 mathie Exp $
//
// $Log: kata5.cc,v $
// Revision 1.3  2004/02/03 07:50:00  mathie
// * Two new 'hashes' - one with a 16-bit space using character pairs and
//   one which will take an MD5 hash and split it into a (parameterised)
//   number of hashes.  The latter is tested with a 16-bit and 24-bit
//   space.
//
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
#include <iostream>

extern "C" {
#include <openssl/md5.h>
};

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

class char_pairs
{
 public:
  static const int map_size;
  
  hash_list operator()(const string& word) const
  {
    hash_list h;
    for(unsigned int i = 0; i < word.size() / 2; i++) {
      h.push_back((static_cast<int>(word[2 * i]) << 8)
                  + (static_cast<int>(word[2 * i + 1])));
    }
    if(word.size() % 2) {
      h.push_back(static_cast<int>(word[word.size() - 1]));
    }
    return h;
  }
};

const int char_pairs::map_size = 65536;

void test_char_pairs_hash()
{
  bloom_filter<char_pairs> bf;
  bf.insert("foo");
  bf.insert("bars");
  bf.insert("bazification");

  // Successful lookups
  BOOST_CHECK(bf.lookup("foo") == true);
  BOOST_CHECK(bf.lookup("bars") == true);
  BOOST_CHECK(bf.lookup("bazification") == true);

  // Try some things not in the dictionary
  BOOST_CHECK(bf.lookup("notindict") == false);

  // A false positive
  BOOST_CHECK(bf.lookup("rso") == true);
  
}

template <size_t hash_bits = 16>
class md5_hash
{
 public:
  static const int map_size;
  
  hash_list operator()(const string& word) const
  {
    unsigned char hash[16 + (hash_bits >> 3) + 1] = {0};
    MD5_CTX context;
    MD5_Init(&context);
    MD5_Update(&context, word.c_str(), word.size());
    MD5_Final(hash, &context);

    cout << "hash = ";
    for(unsigned int i  = 0; i < 16; i++) {
      cout << hex << static_cast<unsigned int>(hash[i]);
    }
    cout << endl;
    
    hash_list h;
    for(unsigned int i = 0; i < 16; i += (hash_bits >> 3)) {
      unsigned int v = 0;
      for(unsigned int j = 0; j < (hash_bits >> 3); j++) {
        v += hash[i + j] << (j << 3);
      }
      h.push_back(v);
    }
    cout << "hash_list = ";
    for(hash_list::const_iterator i = h.begin(); i != h.end(); i++) {
      cout << hex << *i << ", ";
    }
    cout << endl;
    
    return h;
  }
};

template <size_t hash_bits>
const int md5_hash<hash_bits>::map_size = (1 << hash_bits);

void test_md5_hash()
{
  bloom_filter<md5_hash<> > bf;
  bf.insert("foo");
  bf.insert("bars");
  bf.insert("bazification");

  // Successful lookups
  BOOST_CHECK(bf.lookup("foo") == true);
  BOOST_CHECK(bf.lookup("bars") == true);
  BOOST_CHECK(bf.lookup("bazification") == true);

  // Try some things not in the dictionary
  BOOST_CHECK(bf.lookup("notindict") == false);

  bloom_filter<md5_hash<24> > bf24;
  bf24.insert("foo");
  bf24.insert("bars");
  bf24.insert("bazification");

  // Successful lookups
  BOOST_CHECK(bf24.lookup("foo") == true);
  BOOST_CHECK(bf24.lookup("bars") == true);
  BOOST_CHECK(bf24.lookup("bazification") == true);

  // Try some things not in the dictionary
  BOOST_CHECK(bf24.lookup("notindict") == false);
}

test_suite *init_unit_test_suite(int argc, char *argv[])
{
  test_suite *t = BOOST_TEST_SUITE("Code Kata 5: Bloom Filters");
  t->add(BOOST_TEST_CASE(&test_split_into_chars_hash));
  t->add(BOOST_TEST_CASE(&test_char_pairs_hash));
  t->add(BOOST_TEST_CASE(&test_md5_hash));
  return t;
}
