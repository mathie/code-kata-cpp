// Code Kata 5: Bloom Filters
//
// $Id: kata5.cc,v 1.6 2004/02/05 13:52:43 mathie Exp $
//
// $Log: kata5.cc,v $
// Revision 1.6  2004/02/05 13:52:43  mathie
// * All use of an individual hash from the hash_list is typed in terms of
//   hash_list::value_type.
// * Generalise the testing functions.
// * Additional (disabled) trace in the md5_hash operator() function.
// * md5_hash now works for values (hash_bits % 8) != 0.
//
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
#include <iomanip>

extern "C" {
#include <openssl/md5.h>
};

using boost::unit_test_framework::test_suite;
using namespace std;

const string dict = "/usr/share/dict/words";

typedef vector<unsigned int> hash_list;

template <class hash_fn,
          typename hash_list::value_type map_size = hash_fn::map_size>
class bloom_filter 
{
  static const hash_list::value_type ms;
  
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

  hash_list::value_type get_map_size() const
  {
    return ms;
  }

  const string& get_hash_name() const
  {
    return hashes.get_name();
  }
};

template <class hash_fn,
          typename hash_list::value_type map_size>
const hash_list::value_type bloom_filter<hash_fn, map_size>::ms = map_size;

class split_into_chars
{
  static const string name;
  
 public:
  static const hash_list::value_type map_size;
  
  hash_list operator()(const string& word) const
  {
    hash_list h;
    for(string::const_iterator i = word.begin(); i != word.end(); i++) {
      h.push_back(static_cast<int>(*i));
    }
    return h;
  }
  const string& get_name() const 
  {
    return name;
  }
  
};

const hash_list::value_type split_into_chars::map_size = 256;
const string split_into_chars::name = "split_into_chars";

class char_pairs
{
  static const string name;
  
 public:
  static const hash_list::value_type map_size;
  
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

  const string& get_name() const 
  {
    return name;
  }
};

const hash_list::value_type char_pairs::map_size = 65536;
const string char_pairs::name = "char_pairs";

template <hash_list::value_type hash_bits = 16>
class md5_hash
{
  // TODO: This currently only works where hash_bits < sizeof(size_t) (the
  // limit of the size of bitset<>).
  BOOST_STATIC_ASSERT(hash_bits < (sizeof(size_t) << 3));

  static const string name;

 public:
  static const hash_list::value_type map_size;

  hash_list operator()(const string& word) const
  {
    const unsigned int md5_len = 16; // bytes
    unsigned char hash[md5_len + (hash_bits >> 3) + 1] = {0};
    MD5_CTX context;
    MD5_Init(&context);
    MD5_Update(&context, word.c_str(), word.size());
    MD5_Final(hash, &context);

#if 0
    clog << "hash = 0x";
    for(unsigned int i  = 0; i < 16; i++) {
      clog << setw(2) << setfill('0') << noshowbase << hex
           << static_cast<unsigned int>(hash[i]);
    }
    clog << endl;
#endif

    hash_list h;
    for(unsigned int start_bit = 0; start_bit < (md5_len << 3);
        start_bit += hash_bits) {
      const unsigned int start_byte = start_bit >> 3;
      const unsigned int end_bit = start_bit + hash_bits - 1;
      const unsigned int end_byte = end_bit >> 3;
      const unsigned int end_byte_bits = (end_bit + 1) % 8;
      
      
      hash_list::value_type v = 0;
      for(unsigned int i = 0; i <= (end_byte - start_byte); i++) {
        v += hash[end_byte - i] << (i << 3);
      }
      v >>= end_byte_bits;
      v &= (1 << hash_bits) - 1;
      assert(v >= 0 && v < (1 << hash_bits));
      
#if 0
      clog << "start_bit = " << dec << start_bit
           << ", start_byte = " << dec << start_byte
           << ", end_byte = " << dec << end_byte
           << ", end_byte_bits = " << dec << end_byte_bits
           << ", v = " << showbase << hex << v << endl;
#endif
      h.push_back(v);
    }

    // There is a greater chance of the last element being zero,
    // particularly if hash_bits causes the hash to be split in such a
    // way that only one or two significant bits are included in that
    // element.  To even things up a little, if it is zero, remove it.
    if(h.back() == 0) {
      h.pop_back();
    }
    
#if 0
    clog << "hash_list = ";
    for(hash_list::const_iterator i = h.begin(); i != h.end(); i++) {
      clog << setw((hash_bits >> 3) + 1) << setfill('0') << showbase
           << hex << *i << ", ";
    }
    clog << endl;
#endif
    
    return h;
  }

  const string& get_name() const 
  {
    return name;
  }
};

template <hash_list::value_type hash_bits>
const hash_list::value_type md5_hash<hash_bits>::map_size = (1 << hash_bits);
template <hash_list::value_type hash_bits>
const string md5_hash<hash_bits>::name = "md5_hash";

/////////////////
// Test functions
/////////////////

template <class hash_fn>
void test_insert_some_words()
{
  bloom_filter<hash_fn> bf;
  bf.insert("foo");
  bf.insert("bar");
  bf.insert("bazification");

  BOOST_MESSAGE(bf.get_hash_name() << " (map_size = " << bf.get_map_size()
                << ") with 3 entries is "
                << bf.saturation() << "% full.");
  
  // Successful lookups
  BOOST_CHECK(bf.lookup("foo") == true);
  BOOST_CHECK(bf.lookup("bar") == true);
  BOOST_CHECK(bf.lookup("bazification") == true);

  // Try some things not in the dictionary
  BOOST_CHECK(bf.lookup("notindict") == false);
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
    BOOST_CHECK(bf.lookup(word)); // No false negatives!
    n_words++;
  }

  BOOST_MESSAGE(bf.get_hash_name() << " (map_size = " << bf.get_map_size()
                << ") with " << n_words << " entries is "
                << bf.saturation() << "% full.");
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
  const unsigned int n_tests = 10000;
  
  unsigned int pos = 0, neg = 0, fpos = 0;
  bloom_filter<hash_fn> bf;
  bf.load_dictionary(dict);

  set<string> real_dict;
  ifstream f(dict.c_str());
  string line;
  while(getline(f, line)) {
    real_dict.insert(line);
  }

  
  for(unsigned int i = 0; i < n_tests; i++) {
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
                << "for " << n_tests << " test cases, there were "
                << neg << " words not in the dictionary,\n"
                << pos << " words verified as being in the dictionary and "
                << fpos << " false positives.");
}

test_suite *init_unit_test_suite(int argc, char *argv[])
{
  srand(time(NULL));
  
  test_suite *t = BOOST_TEST_SUITE("Code Kata 5: Bloom Filters");
  t->add(BOOST_TEST_CASE(&test_insert_some_words<split_into_chars>));
  t->add(BOOST_TEST_CASE(&test_insert_some_words<char_pairs>));
  t->add(BOOST_TEST_CASE(&test_insert_some_words<md5_hash<8> >));
  t->add(BOOST_TEST_CASE(&test_insert_some_words<md5_hash<16> >));
  t->add(BOOST_TEST_CASE(&test_insert_some_words<md5_hash<17> >));
  t->add(BOOST_TEST_CASE(&test_insert_some_words<md5_hash<18> >));
  t->add(BOOST_TEST_CASE(&test_insert_some_words<md5_hash<19> >));
  t->add(BOOST_TEST_CASE(&test_insert_some_words<md5_hash<20> >));
  t->add(BOOST_TEST_CASE(&test_insert_some_words<md5_hash<21> >));
  t->add(BOOST_TEST_CASE(&test_insert_some_words<md5_hash<22> >));
  t->add(BOOST_TEST_CASE(&test_insert_some_words<md5_hash<23> >));
  t->add(BOOST_TEST_CASE(&test_insert_some_words<md5_hash<24> >));
  t->add(BOOST_TEST_CASE(&test_dictionary<md5_hash<18> >));
  t->add(BOOST_TEST_CASE(&test_dictionary<md5_hash<19> >));
  t->add(BOOST_TEST_CASE(&test_dictionary<md5_hash<20> >));
  t->add(BOOST_TEST_CASE(&test_dictionary<md5_hash<21> >));
  t->add(BOOST_TEST_CASE(&test_dictionary<md5_hash<22> >));
  t->add(BOOST_TEST_CASE(&test_random_words<md5_hash<18> >));
  t->add(BOOST_TEST_CASE(&test_random_words<md5_hash<19> >));
  t->add(BOOST_TEST_CASE(&test_random_words<md5_hash<20> >));
  t->add(BOOST_TEST_CASE(&test_random_words<md5_hash<21> >));
  t->add(BOOST_TEST_CASE(&test_random_words<md5_hash<22> >));
  t->add(BOOST_TEST_CASE(&test_random_words<md5_hash<24> >));
  return t;
}
