// Code Kata 5: Bloom Filters
//
// $Id: kata5.cc,v 1.5 2004/02/05 06:04:00 mathie Exp $
//
// $Log: kata5.cc,v $
// Revision 1.5  2004/02/05 06:04:00  mathie
// * Replace output messages in the test suite with BOOST_MESSAGE() so
//   the verbosity of the test run is controlled by the test suite itself.
// * Include a note about the current constraints of the md5_hash hash
//   size, backing it up with static assertions.
// * Parameterise test_md5_hash(), test_dictionary and test_random_words.
//
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
#include <boost/static_assert.hpp>

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

  BOOST_MESSAGE("bloom_filter<split_into_chars> with 3 entries is "
                << bf.saturation() << "% full.");
  
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

  BOOST_MESSAGE("bloom_filter<char_pairs> with 3 entries is "
                << bf.saturation() << "% full.");
  
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
  // TODO: This currently only works where (hash_bits % 8) == 0 and
  // hash_bits < 32, mostly because of the for loop in operator() which
  // turns the MD5 string into a list of appropriately-sized hashes.
  BOOST_STATIC_ASSERT(hash_bits % 8 == 0);
  BOOST_STATIC_ASSERT(hash_bits < 32);
  
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
    clog << "hash = ";
    for(unsigned int i  = 0; i < 16; i++) {
      clog << hex << static_cast<unsigned int>(hash[i]);
    }
    clog << endl;
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
    clog << "hash_list = ";
    for(hash_list::const_iterator i = h.begin(); i != h.end(); i++) {
      clog << hex << *i << ", ";
    }
    clog << endl;
#endif
    
    return h;
  }
};

template <size_t hash_bits>
const int md5_hash<hash_bits>::map_size = (1 << hash_bits);

template <size_t hash_bits>
void test_md5_hash()
{
  bloom_filter<md5_hash<hash_bits> > bf;
  bf.insert("foo");
  bf.insert("bars");
  bf.insert("bazification");

  BOOST_MESSAGE("bloom_filter<md5_hash<" << hash_bits
                << "> > with 3 entries is " << bf.saturation() << "% full.");

  // Successful lookups
  BOOST_CHECK_EQUAL(bf.lookup("foo"), true);
  BOOST_CHECK_EQUAL(bf.lookup("bars"), true);
  BOOST_CHECK_EQUAL(bf.lookup("bazification"), true);

  // Try some things not in the dictionary
  BOOST_CHECK_EQUAL(bf.lookup("notindict"), false);
}

template <class hash_fn>
void test_dictionary()
{
  bloom_filter<hash_fn> bf;
  bf.load_dictionary(dict);
  ifstream f(dict.c_str());
  string word;
  unsigned int n_words = 0;
  while(getline(f, word)) {
    BOOST_CHECK_EQUAL(bf.lookup(word), true); // No false negatives!
    n_words++;
  }

  BOOST_MESSAGE(typeid(bf).name() << " with " << n_words
                << " entries is " << bf.saturation() << "% full.");
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

template<class hash_fn>
void test_random_words()
{
  unsigned int pos = 0, neg = 0, fpos = 0;
  const unsigned int tests = 10000;
  
  bloom_filter<hash_fn> bf;
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
      if(real_dict.find(rword) == real_dict.end()) {
        BOOST_WARN_MESSAGE(true, "False positive found: " << rword);
        fpos++;
      } else {
        pos++;
      }
    } else {
      neg++;
    }
  }

  BOOST_MESSAGE("With hash function " << typeid(hash_fn).name() << ",\n"
                << "for " << tests << " test cases, there were "
                << neg << " words not in the dictionary,\n"
                << pos << " words verified as being in the dictionary and "
                << fpos << " false positives.");
}

test_suite *init_unit_test_suite(int argc, char *argv[])
{
  srand(time(NULL));
  
  test_suite *t = BOOST_TEST_SUITE("Code Kata 5: Bloom Filters");
  t->add(BOOST_TEST_CASE(&test_split_into_chars_hash));
  t->add(BOOST_TEST_CASE(&test_char_pairs_hash));
  t->add(BOOST_TEST_CASE(&test_md5_hash<8>));
  t->add(BOOST_TEST_CASE(&test_md5_hash<16>));
  t->add(BOOST_TEST_CASE(&test_md5_hash<24>));
  t->add(BOOST_TEST_CASE(&test_dictionary<md5_hash<8> >));
  t->add(BOOST_TEST_CASE(&test_dictionary<md5_hash<16> >));
  t->add(BOOST_TEST_CASE(&test_dictionary<md5_hash<24> >));
  t->add(BOOST_TEST_CASE(&test_random_words<md5_hash<24> >));
  return t;
}
