// Code Kata 5: Bloom Filters
//
// $Id: kata5.cc,v 1.4 2004/02/03 18:17:25 mathie Exp $
//
// $Log: kata5.cc,v $
// Revision 1.4  2004/02/03 18:17:25  mathie
// * Load /usr/share/dict/words into various-sized bloom filters with
//   various hash functions.
// * Provide a function which returns the percentage saturation of a
//   particular filter.
// * Implement the random-word-generation test, displaying the number of
//   negative hits, positive hits and false positives.
//
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
#include <fstream>
#include <set>

extern "C" {
#include <openssl/md5.h>
};

using boost::unit_test_framework::test_suite;
using namespace std;

const string dict = "/usr/share/dict/words";

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

  void load_dictionary(const string& dictfile)
  {
    ifstream d(dictfile.c_str());
    string line;
    while(getline(d, line)) {
      insert(line);
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

  unsigned int saturation() const
  {
    return (map.count() * 100) / map_size;
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

  cout << "bloom_filter<split_into_chars> with 3 entries is "
       << bf.saturation() << "% full." << endl;
  
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

  cout << "bloom_filter<char_pairs> with 3 entries is "
       << bf.saturation() << "% full." << endl;
  
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

#if 0
    cout << "hash = ";
    for(unsigned int i  = 0; i < 16; i++) {
      cout << hex << static_cast<unsigned int>(hash[i]);
    }
    cout << endl;
#endif

    hash_list h;
    for(unsigned int i = 0; i < 16; i += (hash_bits >> 3)) {
      unsigned int v = 0;
      for(unsigned int j = 0; j < (hash_bits >> 3); j++) {
        v += hash[i + j] << (j << 3);
      }
      h.push_back(v);
    }
    
#if 0
    cout << "hash_list = ";
    for(hash_list::const_iterator i = h.begin(); i != h.end(); i++) {
      cout << hex << *i << ", ";
    }
    cout << endl;
#endif
    
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

  cout << "bloom_filter<md5_hash<16> > with 3 entries is "
       << bf.saturation() << "% full." << endl;
  
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

  cout << "bloom_filter<md5_hash<24> > with 3 entries is "
       << bf.saturation() << "% full." << endl;
  
  // Successful lookups
  BOOST_CHECK(bf24.lookup("foo") == true);
  BOOST_CHECK(bf24.lookup("bars") == true);
  BOOST_CHECK(bf24.lookup("bazification") == true);

  // Try some things not in the dictionary
  BOOST_CHECK(bf24.lookup("notindict") == false);
}

void test_dictionary()
{
  bloom_filter<md5_hash<8> > bf8;
  bf8.load_dictionary(dict);
  bloom_filter<md5_hash<16> > bf16;
  bf16.load_dictionary(dict);
  bloom_filter<md5_hash<24> > bf24;
  bf24.load_dictionary(dict);

  ifstream f(dict.c_str());
  string word;
  unsigned int n_words = 0;
  while(getline(f, word)) {
    BOOST_CHECK(bf8.lookup(word) == true); // No false negatives!
    BOOST_CHECK(bf16.lookup(word) == true); // No false negatives!
    BOOST_CHECK(bf24.lookup(word) == true); // No false negatives!
    n_words++;
  }

  cout << "bloom_filter<md5_hash<8> > with " << n_words << " entries is "
       << bf8.saturation() << "% full." << endl;
  cout << "bloom_filter<md5_hash<16> > with " << n_words << " entries is "
       << bf16.saturation() << "% full." << endl;
  cout << "bloom_filter<md5_hash<24> > with " << n_words << " entries is "
       << bf24.saturation() << "% full." << endl;
}

template <size_t len>
string random_word()
{
  char word[len + 1] = {0};

  for (unsigned int i = 0; i < len; i++) {
    word[i] = (rand() % 26) + 'a';
  }
  return string(word);
}

void test_random_words()
{
  unsigned int pos = 0, neg = 0, fpos = 0;
  const unsigned int tests = 10000;
  
  bloom_filter<md5_hash<24> > bf;
  bf.load_dictionary(dict);

  set<string> real_dict;
  ifstream f(dict.c_str());
  string line;
  while(getline(f, line)) {
    real_dict.insert(line);
  }

  
  for(unsigned int i = 0; i < tests; i++) {
    string rword = random_word<5>();
    if(bf.lookup(rword)) {
      cout << rword << endl;
      if(real_dict.find(rword) == real_dict.end()) {
        fpos++;
      } else {
        pos++;
      }
    } else {
      neg++;
    }
  }

  cout << "For " << tests << " test cases, there were "
       << neg << " words not in the dictionary, " << endl
       << pos << " words verified as being in the dictionary and "
       << fpos << " false positives." << endl;
}

test_suite *init_unit_test_suite(int argc, char *argv[])
{
  srand(time(NULL));
  
  test_suite *t = BOOST_TEST_SUITE("Code Kata 5: Bloom Filters");
  t->add(BOOST_TEST_CASE(&test_split_into_chars_hash));
  t->add(BOOST_TEST_CASE(&test_char_pairs_hash));
  t->add(BOOST_TEST_CASE(&test_md5_hash));
  t->add(BOOST_TEST_CASE(&test_dictionary));
  t->add(BOOST_TEST_CASE(&test_random_words));
  return t;
}
