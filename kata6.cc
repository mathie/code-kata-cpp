// Code Kata 6: Anagrams
//
// $Id: kata6.cc,v 1.3 2004/02/07 09:09:47 mathie Exp $
//
// $Log: kata6.cc,v $
// Revision 1.3  2004/02/07 09:09:47  mathie
// * Normalise case, ignore non-alpha characters (instead of assert()ing on
//   them).
// * When output is generated, ignore words in map which have no anagrams.
// * Test functions to generate anagram lists from dictionary files.
//
// Revision 1.2  2004/02/07 07:04:15  mathie
// * Very basic implementation so far - a class to encapsulate word
//   'equivalence' (ie they share all the same characters) and a class
//   which will collate equivalent words.  The interface to the collation
//   class needs work to make it useful.
//
// Revision 1.1  2004/02/05 16:37:36  mathie
// Initial revision
//

// Problem definition:
// http://www.pragprog.com/pragdave/Practices/Kata/KataSix.rdoc

#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include <list>
#include <iostream>
#include <fstream>

using boost::unit_test_framework::test_suite;
using namespace std;

class word_rep 
{
  unsigned int char_count[26];
  unsigned int *char_count_begin, *char_count_end;
  const string word;
  
 public:
  word_rep(const string& word)
    : char_count_begin(char_count),
      char_count_end(char_count + sizeof(char_count)/sizeof(*char_count)),
      word(word)
  {
    fill(char_count_begin, char_count_end, 0);
    for(string::const_iterator it = word.begin(); it != word.end(); it++) {
      if(!isalpha(*it)) {
        BOOST_MESSAGE("Word '" << word << "' contains non-alpha characters.  "
                      << "Ignoring them...");
        continue;
      }
      char_count[*it - (islower(*it) ? 'a' : 'A')]++;
    }
  }

  // Copy-constructor, required to explicitly copy char_count.
  word_rep(const word_rep& c)
    : char_count_begin(char_count),
      char_count_end(char_count + sizeof(char_count)/sizeof(*char_count)),
      word(c.word)
  {
    copy(c.char_count_begin, c.char_count_end, char_count_begin);
  }
  
  const string& get_word() const
  {
    return word;
  }
  
  bool operator==(const word_rep& rhs) const
  {
    return equal(char_count_begin, char_count_end, rhs.char_count_begin);
  }

  bool operator!=(const word_rep& rhs) const
  {
    return !operator==(rhs);
  }

  bool operator<(const word_rep& rhs) const
  {
    return lexicographical_compare(char_count_begin, char_count_end,
                                   rhs.char_count_begin, rhs.char_count_end);
  }
};

class anagrams 
{
  typedef set<string> word_list;
  typedef map<word_rep, word_list> anagram_list;
  
  anagram_list al;

 public:
  void insert(const string& w) 
  {
    word_rep word(w);
    
#if 0
    anagram_list::const_iterator it = al.find(word);
    if(it != al.end()) {
      clog << "Word '"<< word.get_word() <<"' is already represented:" << endl
           << "    First word: " << it->first.get_word() << endl
           << "    ";
      copy(it->second.begin(), it->second.end(),
           ostream_iterator<string>(clog, " "));
      clog << endl;
    } else {
      clog << "Word '" << word.get_word() << "' has no anagrams.  Inserting."
           << endl;
    }
#endif
    al[word].insert(word.get_word());
  }

  friend ostream& operator<<(ostream& s, const anagrams& a) 
  {
    for(anagram_list::const_iterator i = a.al.begin(); i != a.al.end(); i++) {
      if(i->second.size() < 2) {
        continue;
      }
      word_list::const_iterator j = i->second.begin();
      s << *j;
      while(++j != i->second.end()) {
        s << " " << *j;
      }
      s << endl;
    }
    return s;
  }
};

// Test functions
void test_word_rep()
{
  word_rep kinship("kinship"), pinkish("pinkish"), enlist("enlist"),
    inlets("inlets"), listen("listen"), silent("silent"), boaster("boaster"),
    boaters("boaters"), borates("borates"), fresher("fresher"),
    refresh("refresh"), sinks("sinks"), skins("skins"), knits("knits"),
    stink("stink"), rots("rots"), sort("sort");

  BOOST_CHECK_EQUAL(kinship.get_word(), "kinship");
  BOOST_CHECK_EQUAL(pinkish.get_word(), "pinkish");
  BOOST_CHECK_EQUAL(enlist.get_word(), "enlist");
  BOOST_CHECK_EQUAL(inlets.get_word(), "inlets");
  BOOST_CHECK_EQUAL(listen.get_word(), "listen");
  BOOST_CHECK_EQUAL(silent.get_word(), "silent");
  BOOST_CHECK_EQUAL(boaster.get_word(), "boaster");
  BOOST_CHECK_EQUAL(boaters.get_word(), "boaters");
  BOOST_CHECK_EQUAL(borates.get_word(), "borates");
  BOOST_CHECK_EQUAL(fresher.get_word(), "fresher");
  BOOST_CHECK_EQUAL(refresh.get_word(), "refresh");
  BOOST_CHECK_EQUAL(sinks.get_word(), "sinks");
  BOOST_CHECK_EQUAL(skins.get_word(), "skins");
  BOOST_CHECK_EQUAL(knits.get_word(), "knits");
  BOOST_CHECK_EQUAL(stink.get_word(), "stink");
  BOOST_CHECK_EQUAL(rots.get_word(), "rots");
  BOOST_CHECK_EQUAL(sort.get_word(), "sort");

  // All permutations of equal words from the above list.
  BOOST_CHECK(kinship == pinkish);
  BOOST_CHECK(pinkish == kinship);
  
  BOOST_CHECK(enlist == inlets);
  BOOST_CHECK(enlist == listen);
  BOOST_CHECK(enlist == silent);
  BOOST_CHECK(inlets == enlist);
  BOOST_CHECK(inlets == listen);
  BOOST_CHECK(inlets == silent);
  BOOST_CHECK(listen == enlist);
  BOOST_CHECK(listen == inlets);
  BOOST_CHECK(listen == silent);
  BOOST_CHECK(silent == enlist);
  BOOST_CHECK(silent == inlets);
  BOOST_CHECK(silent == listen);
  
  BOOST_CHECK(boaster == boaters);
  BOOST_CHECK(boaster == borates);
  BOOST_CHECK(boaters == borates);
  BOOST_CHECK(boaters == boaster);
  BOOST_CHECK(borates == boaster);
  BOOST_CHECK(borates == boaters);

  BOOST_CHECK(fresher == refresh);
  BOOST_CHECK(refresh == fresher);

  BOOST_CHECK(sinks == skins);
  BOOST_CHECK(skins == sinks);

  BOOST_CHECK(knits == stink);
  BOOST_CHECK(stink == knits);

  BOOST_CHECK(rots == sort);
  BOOST_CHECK(sort == rots);

  // Check some words which are not equal
  BOOST_CHECK(kinship != inlets);
  BOOST_CHECK(knits != skins);

  // Check some orderings (required if want nice fast searching with a
  // map or somesuch).
  BOOST_CHECK(!(stink < knits));
  BOOST_CHECK(stink < skins);
  BOOST_CHECK(rots < knits);
  
}

void test_anagrams()
{
  const string l[] = { "kinship", "pinkish",
                       "enlist", "inlets", "listen", "silent",
                       "boaster", "boaters", "borates",
                       "fresher", "refresh",
                       "sinks", "skins",
                       "knits", "stink",
                       "rots", "sort" };
  const size_t l_sz = sizeof l / sizeof *l;
  anagrams a;

  for(unsigned int i = 0; i < l_sz; i++) {
    a.insert(l[i]);
  }

  cout << a;
}

void load_dictionary(pair<string, string> args)
{
  const string& infile = args.first;
  const string& outfile = args.second;

  BOOST_MESSAGE("Retrieving from " << infile << ", writing to " << outfile);
  
  ifstream in(infile.c_str());
  string word;
  anagrams a;
  
  while(getline(in, word)) {
    a.insert(word);
  }

  ofstream out(outfile.c_str());
  out << a;
}

class test_dictionaries : public test_suite
{
  list<pair<string, string> > dictionaries;
 public:
  test_dictionaries()
  {
    dictionaries.push_back(make_pair("wordlist.txt", "wordlist.out"));
    dictionaries.push_back(make_pair("/usr/share/dict/words", "maindict.out"));
    
    add(BOOST_PARAM_TEST_CASE(load_dictionary, dictionaries.begin(),
                              dictionaries.end()));
  }
};

test_suite *init_unit_test_suite(int argc, char *argv[])
{
  test_suite *t = BOOST_TEST_SUITE("Code Kata 6: Anagrams");
  t->add(BOOST_TEST_CASE(test_word_rep));
  t->add(BOOST_TEST_CASE(test_anagrams));
  t->add(new test_dictionaries);
  return t;
}
