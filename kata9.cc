// Code Kata 9: Back to the Checkout
//
// $Id: kata9.cc,v 1.2 2004/02/09 12:35:30 mathie Exp $
//
// $Log: kata9.cc,v $
// Revision 1.2  2004/02/09 12:35:30  mathie
// * Sketch out the interface to the checkout class.
// * Implement the test suite, as suggested in the problem definition.
//
// Revision 1.1  2004/02/09 11:54:26  mathie
// Initial revision
//

// Problem definition:
// http://www.pragprog.com/pragdave/Practices/Kata/KataNine.rdoc

#include <boost/test/unit_test.hpp>

#include <string>

using boost::unit_test_framework::test_suite;
using namespace std;

typedef char item;

class checkout_rules
{
};

class checkout
{
  checkout_rules rules;
  unsigned int t;
  
 public:
  checkout(const checkout_rules& r)
    : rules(r), t(0)
  {
  }

  unsigned int total() 
  {
    return t;
  }

  void scan(const item& i) 
  {
  }
};

template<typename C>
unsigned int price_cart(const C& cart, const checkout_rules& rules) 
{
  checkout co(rules);
  for(typename C::const_iterator it = cart.begin(); it != cart.end(); it++) {
    co.scan(*it);
  }
  return co.total();
}

void test_totals()
{
  checkout_rules r;
  BOOST_CHECK_EQUAL(  0U, price_cart(string(""),       r));
  BOOST_CHECK_EQUAL( 50U, price_cart(string("A"),      r));
  BOOST_CHECK_EQUAL( 80U, price_cart(string("AB"),     r));
  BOOST_CHECK_EQUAL(115U, price_cart(string("CDBA"),   r));
  
  BOOST_CHECK_EQUAL(100U, price_cart(string("AA"),     r));
  BOOST_CHECK_EQUAL(130U, price_cart(string("AAA"),    r));
  BOOST_CHECK_EQUAL(180U, price_cart(string("AAAA"),   r));
  BOOST_CHECK_EQUAL(230U, price_cart(string("AAAAA"),  r));
  BOOST_CHECK_EQUAL(260U, price_cart(string("AAAAAA"), r));
  
  BOOST_CHECK_EQUAL(160U, price_cart(string("AAAB"),   r));
  BOOST_CHECK_EQUAL(175U, price_cart(string("AAABB"),  r));
  BOOST_CHECK_EQUAL(190U, price_cart(string("AAABBD"), r));
  BOOST_CHECK_EQUAL(190U, price_cart(string("DABABA"), r));
}

void test_incremental()
{
  checkout_rules r;
  checkout co(r);
  BOOST_CHECK_EQUAL(0U, co.total());
  co.scan('A'); BOOST_CHECK_EQUAL( 50U, co.total());
  co.scan('B'); BOOST_CHECK_EQUAL( 80U, co.total());
  co.scan('A'); BOOST_CHECK_EQUAL(130U, co.total());
  co.scan('A'); BOOST_CHECK_EQUAL(160U, co.total());
  co.scan('B'); BOOST_CHECK_EQUAL(175U, co.total());
}

test_suite *init_unit_test_suite(int argc, char *argv[])
{
  test_suite *t = BOOST_TEST_SUITE("Code Kata 9: Back to the Checkout");
  t->add(BOOST_TEST_CASE(&test_totals));
  t->add(BOOST_TEST_CASE(&test_incremental));
  return t;
}
