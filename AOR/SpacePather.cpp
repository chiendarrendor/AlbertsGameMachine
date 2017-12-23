#include "SpacePather.hpp"
#include "SpaceInfo.hpp"
#include "SpaceInfoMap.hpp"
#include <iostream>

SpacePather::Finger::Finger(const SpacePather::Finger &i_Basis,const SpaceInfo &i_CurSpace) :
  m_History(i_Basis.m_History)
{
  m_History.push_back(&i_CurSpace);
}

SpacePather::Finger::Finger(const SpaceInfo &i_CurSpace)
{
  m_History.push_back(&i_CurSpace);
}

const std::vector<const SpaceInfo *> &SpacePather::Finger::GetHistory() const
{
  return m_History;
}

const SpaceInfo &SpacePather::Finger::GetCurSpace() const
{
  return *(m_History[m_History.size()-1]);
}

SpacePather::SpacePather(const SpaceInfoMap &i_SpaceInfoMap)
{
  SpaceInfoMap::const_iterator sit;
  for (sit = i_SpaceInfoMap.begin() ; sit != i_SpaceInfoMap.end() ; ++sit)
  {
    SpacePather::Finger *pNewFinger = new SpacePather::Finger(*sit->second);
    m_Fingers.push_back(pNewFinger);
  }
}

SpacePather::~SpacePather()
{
  size_t i;
  for (i = 0 ; i < m_Fingers.size() ; ++i)
  {
    delete m_Fingers[i];
  }
}

const std::vector<const SpacePather::Finger *> &SpacePather::GetFingers() const
{
  return m_Fingers;
}

void SpacePather::RegisterPathFilter(const PathFilter &i_pf)
{
  m_PathFilters.push_back(&i_pf);
}

void SpacePather::RegisterFinalPathFilter(const PathFilter &i_pf)
{
  m_FinalPathFilters.push_back(&i_pf);
}

void SpacePather::RegisterGrowFilter(const GrowFilter &i_gf)
{
  m_GrowFilters.push_back(&i_gf);
}


bool SpacePather::IsPathLegal(const Finger &i_Finger) const
{

  if (m_PathFilters.size() == 0) return true;

	std::vector<const PathFilter *>::const_iterator pathit;
	for (pathit = m_PathFilters.begin() ; pathit != m_PathFilters.end() ; ++pathit)
	{
		if ((**pathit)(i_Finger)) return true;
	}

	return false;
}

bool SpacePather::IsFinalPathLegal(const Finger &i_Finger) const
{
  if (m_FinalPathFilters.size() == 0) return true;

	std::vector<const PathFilter *>::const_iterator pathit;
	for (pathit = m_FinalPathFilters.begin() ; pathit != m_FinalPathFilters.end() ; ++pathit)
	{
		if ((**pathit)(i_Finger)) return true;
	}

	return false;
}



bool SpacePather::IsSpaceLegal(const SpaceInfo &i_Space) const
{
	std::vector<const GrowFilter *>::const_iterator growit;
	for (growit = m_GrowFilters.begin() ; growit != m_GrowFilters.end() ; ++growit)
	{
		if (!(**growit)(i_Space)) return false;
	}

	return true;
}

class FingerHolder
{
public:
  FingerHolder() : m_pLandFinger(NULL),m_pCoastFinger(NULL),m_pSeaFinger(NULL) {}
  bool AddFinger(const SpacePather::Finger *i_pFinger)
  {
    size_t i;
    int numland = 0;
    int numcoast = 0;
    int numsea = 0;
    for (i = 0 ; i < i_pFinger->GetHistory().size() ; ++i)
    {
      switch(i_pFinger->GetHistory()[i]->GetSpaceType())
      {
      case LAND: numland++; break;
      case COAST: numcoast++; break;
      default: numsea++ ; break;
      }
    }
    
    if (numsea > 0)
    {
      if (m_pSeaFinger == NULL)
      {
        m_pSeaFinger = i_pFinger;
        return true;
      }
      else
      {
        return false;
      }
    }
    if (numcoast > 0)
    {
      if (m_pCoastFinger == NULL)
      {
        m_pCoastFinger = i_pFinger;
        return true;
      }
      else
      {
        return false;
      }
    }
    if (m_pLandFinger == NULL)
    {
      m_pLandFinger = i_pFinger;
      return true;
    }
    return false;
  }

  const SpacePather::Finger *GetLandFinger() const { return m_pLandFinger; }
  const SpacePather::Finger *GetCoastFinger() const { return m_pCoastFinger; }
  const SpacePather::Finger *GetSeaFinger() const { return m_pSeaFinger; }

private:
  const SpacePather::Finger *m_pLandFinger;
  const SpacePather::Finger *m_pCoastFinger;
  const SpacePather::Finger *m_pSeaFinger;
};




void SpacePather::Grow()
{
  std::map<std::string,FingerHolder> spacefingers;
  std::map<std::string,FingerHolder>::iterator fingermapit;
  std::vector<const Finger *> nextround;

  std::vector<const Finger *>::iterator fingervectorit;

  for (fingervectorit = m_Fingers.begin() ; fingervectorit != m_Fingers.end() ; ++fingervectorit)
  {
    spacefingers[(*fingervectorit)->GetCurSpace().GetName()].AddFinger(*fingervectorit);
  }

  while(m_Fingers.size() != 0)
  {
    for (fingervectorit = m_Fingers.begin() ; fingervectorit != m_Fingers.end() ; ++fingervectorit)
    {
      const Finger *pFinger = *fingervectorit;

      const SpaceInfoMap &neighbors = pFinger->GetCurSpace().GetLinks();
      SpaceInfoMap::const_iterator neighborit;

      for (neighborit = neighbors.begin() ; neighborit != neighbors.end() ; ++neighborit)
      {
        const SpaceInfo *pNeighborSpace = neighborit->second;

        if (!IsSpaceLegal(*pNeighborSpace))
        {
          continue;
        }
        
        Finger *pNewFinger = new Finger(*pFinger,*pNeighborSpace);
        
        if (!IsPathLegal(*pNewFinger))
        {
          delete pNewFinger;
          continue;
        }

        if (spacefingers[pNeighborSpace->GetName()].AddFinger(pNewFinger))
        {
          nextround.push_back(pNewFinger);
        }
        else
        {
          delete pNewFinger;
        }
      }
    }

    m_Fingers = nextround;
    nextround.clear();
  }

  for (fingermapit = spacefingers.begin() ; fingermapit != spacefingers.end() ; ++fingermapit)
  {
    FingerHolder &fholder = fingermapit->second;

    if (fholder.GetLandFinger() && IsFinalPathLegal(*(fholder.GetLandFinger())))
    {
      m_Fingers.push_back(fholder.GetLandFinger());
      delete fholder.GetCoastFinger();
      delete fholder.GetSeaFinger();
      continue;
    }
    delete fholder.GetLandFinger();

    if (fholder.GetSeaFinger() && IsFinalPathLegal(*(fholder.GetSeaFinger())))
    {
      m_Fingers.push_back(fholder.GetSeaFinger());
      delete fholder.GetCoastFinger();
      continue;
    }
    delete fholder.GetSeaFinger();

    if (fholder.GetCoastFinger() && IsFinalPathLegal(*(fholder.GetCoastFinger())))
    {
      m_Fingers.push_back(fholder.GetCoastFinger());
      continue;
    }
    delete fholder.GetCoastFinger();
  }
}
        
std::ostream &operator<<(std::ostream &o,const SpacePather &i_sp)
{
  const std::vector<const SpacePather::Finger *> &fingers = i_sp.GetFingers();

  o << "SpacePather:" << std::endl;
  for (size_t i = 0 ; i < fingers.size() ; ++i)
  {
    o << "   " << *(fingers[i]) << std::endl;
  }
  return o;
}


std::ostream &operator<<(std::ostream &o,const SpacePather::Finger &i_Finger)
{
  const std::vector<const SpaceInfo *> &spaces = i_Finger.GetHistory();
  for (size_t i = 0 ; i < spaces.size() ; ++i)
  {
    if (i != 0)
    {
      o << " -> ";
    }
    o << spaces[i]->GetName();
  }
  return o;
}

