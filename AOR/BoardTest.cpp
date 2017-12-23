#include "GameBoard.hpp"
#include <iostream>
#include "CommodityInfo.hpp"
#include "SpacePather.hpp"
#include "SpaceInfo.hpp"

class LandGrowFilter : public SpacePather::GrowFilter
{
public:
  virtual bool operator()(const SpaceInfo &i_Space) const { return i_Space.GetSpaceType() == LAND; }
};

class WalkPathFilter : public SpacePather::PathFilter
{
public:
  virtual bool operator()(const SpacePather::Finger &i_Finger) const
  {
    size_t i;
    int landcount = 0;
    for (i = 0 ; i < i_Finger.GetHistory().size() ; ++i)
    {
      if (i_Finger.GetHistory()[i]->GetSpaceType() == LAND) 
      {
        landcount++;
      }
      else
      {
        return false;
      }
    }
    return landcount <= 2;
  }
};

class CaravanPathFilter : public SpacePather::PathFilter
{
public:
  virtual bool operator()(const SpacePather::Finger &i_Finger) const
  {
    size_t i;
    int landcount = 0;
    for (i = 0 ; i < i_Finger.GetHistory().size() ; ++i)
    {
      if (i_Finger.GetHistory()[i]->GetSpaceType() == LAND)
      {
        landcount++;
      }
      else
      {
        return false;
      }
    }
    if (landcount <= 2) return true;
    if (landcount > 3) return false;
    
    //    if (i_Finger.GetHistory()[1]->GetName() == "Paris") return false;
    return true;
  }
};

class GalleyPathFilter : public SpacePather::PathFilter
{
public:
  GalleyPathFilter(int i_GalleySize,bool i_Heavens) : m_GalleySize(i_GalleySize),m_Heavens(i_Heavens) {}
  virtual bool operator()(const SpacePather::Finger &i_Finger) const
  {
    int landcount = 0;
    int coastcount = 0;
    int seacount = 0;
    size_t i;
    for (i = 0 ; i < i_Finger.GetHistory().size() ; ++i)
    {
      SpaceType stype = i_Finger.GetHistory()[i]->GetSpaceType();
      switch(stype)
      {
      case LAND: 
        if (landcount == 2) return false;
        landcount++; 
        break;
      case COAST: 
        if (coastcount == (2 * m_GalleySize) + 1) return false;
        coastcount++; 
        if (landcount == 2) return false;
        break;
      case SEA:
        if (coastcount == (2 * m_GalleySize) + 1) return false;
        if (!m_Heavens) return false;
        if (seacount == 1) return false;
        seacount++;
        coastcount++;
        break;
      default: return false;
      }
    }
    return true;
  }
private:
  int m_GalleySize;
  bool m_Heavens;
};

class SafeVoyageFinalPathFilter : public SpacePather::PathFilter
{
public:
  virtual bool operator()(const SpacePather::Finger &i_Finger) const
  {
    return i_Finger.GetCurSpace().GetSpaceType() == LAND;
  }
};






int main(int argc,char **argv)
{
  CommodityInfo ci;

  GameBoard gb("../../data/Age_Of_Renaissance/AorMap.txt",ci);

  if (!gb.LoadOk())
  {
    std::cout << "Load Failed." << std::endl;
  }

  //  gb.PrintAttributes();

  const SpaceInfoMap &allspaces = gb.GetSpaceInfoMap();

  SpaceInfoMap bruges;

  SpaceInfoMap::const_iterator findit;
  //findit = allspaces.find("Bruges");  bruges.Insert(*(findit->second));
  //findit = allspaces.find("Bordeaux");  bruges.Insert(*(findit->second));
  //findit = allspaces.find("Esseg");  bruges.Insert(*(findit->second));
  findit = allspaces.find("Dubrovnik");  bruges.Insert(*(findit->second));

  SpacePather spath(bruges);
  //LandGrowFilter lgf; spath.RegisterGrowFilter(lgf);
  //WalkPathFilter wpf; spath.RegisterPathFilter(wpf);
  //CaravanPathFilter cpf; spath.RegisterPathFilter(cpf);
  //GalleyPathFilter gpf(3,true); spath.RegisterPathFilter(gpf);

  SafeVoyageFinalPathFilter svfpf; spath.RegisterFinalPathFilter(svfpf);


  std::cout << spath;

  spath.Grow();

  std::cout << spath;



}
