#include "SpaceState.hpp"
#include <stdexcept>

#include "GameBoard.hpp"

class AOREventGenerator;
class Players;
class CommodityInfo;

class BoardState
{
public:
  BoardState(const GameBoard &i_GameBoard);

  void SetDominance(const std::string &i_Capital, const std::string &i_Space,bool i_IsOld);

  const std::map<std::string,SpaceState> &GetSpaceStates() const;

  // this broadcasts the current info about the current space.
  void BroadcastSpaceInfo(const AOREventGenerator &i_egen,
                          const std::string &i_Space) const;
  // this broadcasts the commodity log info about the commodity(ies) of the
  // space listed.
  void BroadcastCommodityLogInfo(const AOREventGenerator &i_egen,
                                 const std::string &i_Space,
                                 const Players &i_PlayerList,
                                 size_t i_numCapitals) const;

  // this unicasts to the specified player the entire board state.
  // (can assume the board was reset prior to the call)
  void SendAllSpaceInfo(const AOREventGenerator &i_egen,
                        const std::string &i_Player) const;

  // this unicasts to the specified player all the Commodity info
  // can assume the board was reset prior to the call
  void SendAllCommodityLogInfo(const AOREventGenerator &i_egen,
                               const std::string &i_Player,
                               const Players &i_PlayerList,
                               size_t i_numCapitals,
                               const CommodityInfo &i_CommodityInfo) const;




private:
  SERIALIZE_FRIENDSHIP;
  SERIALIZE_SAVE_FUNC
  {
    SERIALIZE(m_SpaceStates);
  }
  SERIALIZE_LOAD_FUNC
  {
    SERIALIZE(m_SpaceStates);
    std::map<std::string,SpaceState>::iterator spaceit;
    for (spaceit = m_SpaceStates.begin() ; spaceit != m_SpaceStates.end() ; ++spaceit)
    {
      const SpaceInfo *pSpace = m_GameBoard.GetSpaceInfoMap().GetSpace(spaceit->first);
      if (!pSpace)
      {
        throw std::out_of_range("Unknown Space to Load!");
      }
      spaceit->second.SetSpaceInfo(pSpace);
    }
  }
  BOOST_SERIALIZATION_SPLIT_MEMBER();

  SpaceState &GetSpaceState(const std::string &i_Space);
  const SpaceState &GetSpaceState(const std::string &i_Space) const;

  mutable std::map<std::string,SpaceState> m_SpaceStates;
  const GameBoard &m_GameBoard;
};

