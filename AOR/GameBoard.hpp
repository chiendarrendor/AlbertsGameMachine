#ifndef GAMEBOARDHPP
#define GAMEBOARDHPP

#include "SpaceInfoMap.hpp"

class CommodityInfo;



class GameBoard
{
public:
	GameBoard(const std::string &i_DataFile,const CommodityInfo &i_rComLog);
	bool LoadOk() const;
	void PrintAttributes() const;
	const SpaceInfoMap &GetSpaceInfoMap() const;
private:
  //	void SetOverseas();
  //	void BuildSatellites();
	SpaceInfoMap m_spaces;
	bool m_loadok;
};

#endif
