#include <set>
#include <vector>
#include "Serialize.hpp"
#include "Players.hpp"
#include "CardManager.hpp"
#include "SpaceState.hpp"
#include "BoardState.hpp"

class AORSet;

const int INITIALSTAKE = 40;


class AORState
{
public:
  // required external interfaces
  AORState(const AORSet &i_Set);

  // my internal interfaces
  Players &GetPlayers();
  const Players &GetPlayers() const;
  CardManager &GetCardManager();
  const CardManager &GetCardManager() const;
  BoardState &GetBoardState();
  const BoardState &GetBoardState() const;

  const AORSet &GetAORSet() const;

  std::string GetLegalBidString(int i_Min,int i_Max) const;

  void PrepareCapitalChoices();
  // during Capital choosing, this will increment as
  // players choose...once that is done, this should be equivalent
  // to # of players

  size_t GetCapitalChooser() const;
  std::string GetCapitalChoiceList() const;
  const std::set<std::string> &GetCapitalChoiceSet() const;
  void ClearCapitalChoice(const std::string &i_CapitalChoice);

  int GetCurTurn() const;
  void SetCurTurn(int i_CurTurn);


private:
  SERIALIZE_FUNC
  {
    SERIALIZE(m_Players);
    SERIALIZE(m_CardManager);
    SERIALIZE(m_CapitalChoices);
    SERIALIZE(m_BoardState);
    SERIALIZE(m_CurTurn);
  }

  Players m_Players;
  CardManager m_CardManager;
  std::set<std::string> m_CapitalChoices;
  BoardState m_BoardState;
  const AORSet &m_AORSet;
  int m_CurTurn;
};

