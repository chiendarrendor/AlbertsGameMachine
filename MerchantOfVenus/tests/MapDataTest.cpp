#include "MapData.hpp"
#include "MapSpace.hpp"

#include <boost/test/unit_test.hpp>

// These tests characterize MapData's XML parsing behavior (boost::property_tree-based).

BOOST_AUTO_TEST_CASE( TestMapDataGood )
{
  MapData md("../tests/mapdata_good.xml");

  BOOST_CHECK(md.GetRegionMap().size() == 1);
  BOOST_CHECK(md.GetRegionMap().find("TestSystem") != md.GetRegionMap().end());
  BOOST_CHECK(md.GetRegionMap().find("TestSystem")->second->m_habitable == true);
  BOOST_CHECK(md.GetRegionMap().find("TestSystem")->second->m_inhabited == false);

  BOOST_CHECK(md.GetSpaceMap().size() == 2);

  MapSpace *sp1 = md.GetSpaceMap().find("TestSystem_loop_1")->second;
  MapSpace *sp2 = md.GetSpaceMap().find("TestSystem_loop_2")->second;
  BOOST_CHECK(sp1->m_type == CITY);
  BOOST_CHECK(sp2->m_type == DOT);
  BOOST_CHECK(sp1->m_adjacentnames.size() == 1);
  BOOST_CHECK(sp1->m_adjacentnames[0].first == "TestSystem_loop_2");
  BOOST_CHECK(sp2->m_adjacentnames.size() == 1);
  BOOST_CHECK(sp2->m_adjacentnames[0].first == "TestSystem_loop_1");
}

BOOST_AUTO_TEST_CASE( TestMapDataRealMap )
{
  // Smoke test + a few concrete counts against the real production map,
  // so a future parser swap can be checked against the same numbers.
  MapData md("../MerchantOfVenusMap.xml");

  size_t numCity = 0, numQbox = 0, numSpaceCity = 0, numPenalty = 0, numDot = 0;
  std::map<std::string,MapSpace *>::const_iterator sit;
  for (sit = md.GetSpaceMap().begin() ; sit != md.GetSpaceMap().end() ; ++sit)
  {
    switch (sit->second->m_type)
    {
      case CITY: ++numCity; break;
      case QBOX: ++numQbox; break;
      case SPACECITY: ++numSpaceCity; break;
      case PENALTY: ++numPenalty; break;
      case DOT: ++numDot; break;
      default: break;
    }
  }

  BOOST_CHECK(numQbox == 24);
  BOOST_CHECK(numSpaceCity == 6);

  BOOST_CHECK(md.GetSpaceMap().find("Galactic Base") != md.GetSpaceMap().end());
  BOOST_CHECK(md.GetSpaceMap().find("Galactic Base")->second->m_type == SPACECITY);
  BOOST_CHECK(md.GetSpaceMap().find("Smoke Ring") != md.GetSpaceMap().end());
  BOOST_CHECK(md.GetSpaceMap().find("Smoke Ring")->second->m_type == SPACECITY);
}

BOOST_AUTO_TEST_CASE( TestMapDataUnknownAttribute )
{
  BOOST_CHECK_THROW(MapData md("../tests/mapdata_unknown_attr.xml"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( TestMapDataMissingRequiredAttribute )
{
  BOOST_CHECK_THROW(MapData md("../tests/mapdata_missing_required.xml"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( TestMapDataIllegalType )
{
  BOOST_CHECK_THROW(MapData md("../tests/mapdata_illegal_type.xml"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( TestMapDataSelfAdjacency )
{
  BOOST_CHECK_THROW(MapData md("../tests/mapdata_self_adjacency.xml"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( TestMapDataNonexistentAdjacency )
{
  BOOST_CHECK_THROW(MapData md("../tests/mapdata_nonexistent_adjacency.xml"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( TestMapDataAsymmetricAdjacency )
{
  BOOST_CHECK_THROW(MapData md("../tests/mapdata_asymmetric_adjacency.xml"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( TestMapDataFlyableNonCity )
{
  BOOST_CHECK_THROW(MapData md("../tests/mapdata_flyable_non_city.xml"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( TestMapDataLoopGap )
{
  BOOST_CHECK_THROW(MapData md("../tests/mapdata_loop_gap.xml"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( TestMapDataDuplicateSpace )
{
  BOOST_CHECK_THROW(MapData md("../tests/mapdata_duplicate_space.xml"), std::runtime_error);
}
