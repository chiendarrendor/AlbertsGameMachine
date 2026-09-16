#include "AppendShuffleDraws.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestAppendShuffleDrawsSimplePermutation )
{
  std::vector<int> draws;
  AppendShuffleDraws(draws,std::vector<char>{'A','B','C','D'},std::vector<char>{'D','A','C','B'});

  std::vector<int> expected{3,2,0,0};
  BOOST_CHECK(draws == expected);
}

BOOST_AUTO_TEST_CASE( TestAppendShuffleDrawsAppendsRatherThanOverwrites )
{
  // callers build one flat script by appending draws from multiple shuffles
  // (and plain die rolls) in call order -- confirm existing entries survive.
  std::vector<int> draws{9};
  AppendShuffleDraws(draws,std::vector<int>{1,2,3},std::vector<int>{3,1,2});
  draws.push_back(5);

  std::vector<int> expected{9,2,1,0,5};
  BOOST_CHECK(draws == expected);
}

BOOST_AUTO_TEST_CASE( TestAppendShuffleDrawsIdentityIsAllZeroes )
{
  std::vector<int> draws;
  AppendShuffleDraws(draws,std::vector<int>{1,2,3,4},std::vector<int>{1,2,3,4});

  std::vector<int> expected{0,0,0,0};
  BOOST_CHECK(draws == expected);
}

BOOST_AUTO_TEST_CASE( TestAppendShuffleDrawsHandlesDuplicateValues )
{
  // duplicate values are interchangeable; any valid tie-break is acceptable,
  // so just confirm the draws it picks are in-range and don't throw.
  std::vector<int> draws;
  AppendShuffleDraws(draws,std::vector<int>{1,1,2,3},std::vector<int>{3,1,1,2});

  BOOST_CHECK(draws.size() == 4);
  BOOST_CHECK(draws[0] >= 0 && draws[0] < 4);
  BOOST_CHECK(draws[1] >= 0 && draws[1] < 3);
  BOOST_CHECK(draws[2] >= 0 && draws[2] < 2);
  BOOST_CHECK(draws[3] == 0);
}

BOOST_AUTO_TEST_CASE( TestAppendShuffleDrawsThrowsOnSizeMismatch )
{
  std::vector<int> draws;
  BOOST_CHECK_THROW(
    AppendShuffleDraws(draws,std::vector<int>{1,2},std::vector<int>{1,2,3}),
    std::runtime_error);
}

BOOST_AUTO_TEST_CASE( TestAppendShuffleDrawsThrowsOnUnmatchedValue )
{
  std::vector<int> draws;
  BOOST_CHECK_THROW(
    AppendShuffleDraws(draws,std::vector<int>{1,2,3},std::vector<int>{1,2,4}),
    std::runtime_error);
}
