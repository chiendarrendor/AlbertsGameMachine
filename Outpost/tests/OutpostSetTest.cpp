#include "OutpostSet.hpp"
#include "SerializeUtility.hpp"
#include <boost/test/auto_unit_test.hpp>
#include <iostream>


BOOST_AUTO_TEST_CASE( OutpostSetConstructor )
{
  OutpostSet ops("dummy");

  BOOST_CHECK_EQUAL(ops.IsValid(),true);
}

