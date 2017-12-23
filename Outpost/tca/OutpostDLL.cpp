#include <sstream>
#include <set>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include "StringUtilities.hpp"
#include "GameBoxDLL.hpp"
#include "OutpostGameInfo.hpp"
#include "DLLGame.hpp"


    
      // this class handles deleting stuff from a players hand.
      // if validateweight is true, class will validate that the deletion put them below
      // their hand limit.
      // if false, class will validate that the deletion spent at least i_cost, and at least i_numresearch.
      class HandDeleter
      {
      public:
        HandDeleter(OutpostGameInfo &i_ogi,Player &i_player,const std::string &i_deletestring,Spend &i_newspend,
	            bool i_validateweight,int i_cost = 0, int i_numresearch = 0) : m_isOk(false)
	{
	  std::vector<Commodity> deletedItems;
	  CommodityHand newhand = i_player.GetCommodityHand();
	  int origover = newhand.GetHandWeight(i_ogi.GetOptions().GetResearchIsSmall(),i_ogi.GetOptions().GetMicroIsSmall())
	               - i_player.GetHandLimit(i_ogi.GetOptions().GetEarlyDiscard());

	  int result = newhand.ApplyDeletes(i_deletestring,deletedItems);
	  switch(result)
          {
	  case -1: i_ogi.UnicastERROR(i_player.GetName(),"Mismatched delete string length"); return;
	  case -2: i_ogi.UnicastERROR(i_player.GetName(),"Illegal characters in delete string"); return;
	  case 0: break; // this is the ok case.
	  default: i_ogi.UnicastERROR(i_player.GetName(),"Unknown error reported by ApplyDeletes"); return;
	  }

	  if (i_validateweight)
	  {
	    i_newspend.SetSpendCost(origover);		       

	    int over = newhand.GetHandWeight(i_ogi.GetOptions().GetResearchIsSmall(),i_ogi.GetOptions().GetMicroIsSmall())
	               - i_player.GetHandLimit(i_ogi.GetOptions().GetEarlyDiscard());

	    if (over > 0)
	    {
	      i_ogi.UnicastERROR(i_player.GetName(),"You need to discard more to get to your hand limit.");
	      return;
	    }
          }
	  else
	  {
            i_newspend.SetSpendCost(i_cost);
	    if (i_player.GetCommodityHand().GetHandValue() - newhand.GetHandValue() < i_cost)
	    {
	      i_ogi.UnicastERROR(i_player.GetName(),"You need to spend more to buy that.");
	      return;
	    }
	    if (i_player.GetCommodityHand().GetNumResearch() - newhand.GetNumResearch() < i_numresearch)
	    {
	      i_ogi.UnicastERROR(i_player.GetName(),"You need to spend more Research resource cards to buy that.");
	      return;
	    }
          }
          m_isOk = true;

	  // if we got here, we can safely discard the cards and make the new hand the player's hand.
	  size_t i;
	  for (i = 0 ; i < deletedItems.size() ; ++i)
	  {
	    i_ogi.GetCommodities().GetDeck(deletedItems[i].GetType()).DiscardCommodity(deletedItems[i]);
	    i_newspend.AddCommodity(deletedItems[i]);
	    
	  }
	  i_player.GetCommodityHand() = newhand;
        }

        bool IsOk() const { return m_isOk; }

      private:	
	bool m_isOk;
      };
    
  
void OutpostGameInfo::UnicastCOMMODITYSTOCK(const std::string &i_PlayerName) const
{
  for( int commType = 1 ; commType != 10 ; ++commType ) {
    UnicastUpdateCOMMODITYSTOCKBycommType(i_PlayerName,commType);
  }
}
void OutpostGameInfo::VaricastCOMMODITYSTOCK(const NameBoolean &i_Variator) const
{
  for( int commType = 1 ; commType != 10 ; ++commType ) {
    VaricastUpdateCOMMODITYSTOCKBycommType(i_Variator,commType);
  }
}
void OutpostGameInfo::BroadcastCOMMODITYSTOCK() const
{
  for( int commType = 1 ; commType != 10 ; ++commType ) {
    BroadcastUpdateCOMMODITYSTOCKBycommType(commType);
  }
}
void OutpostGameInfo::UnicastUpdateCOMMODITYSTOCKBycommType(const std::string &i_PlayerName,
      const int& commType) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeCOMMODITYSTOCKMessage(commType));
}
void OutpostGameInfo::VaricastUpdateCOMMODITYSTOCKBycommType(const NameBoolean &i_Variator,
      const int& commType) const
{
  m_OutputPort.VariCast(i_Variator,MakeCOMMODITYSTOCKMessage(commType));
}
void OutpostGameInfo::BroadcastUpdateCOMMODITYSTOCKBycommType(
      const int& commType) const
{
  m_OutputPort.BroadCast(MakeCOMMODITYSTOCKMessage(commType));
}

std::string OutpostGameInfo::MakeCOMMODITYSTOCKMessage(const int& commType) const
{
  std::ostringstream oss;
  oss << "COMMODITYSTOCK"
  << ',' << UnCommaStringify(commType)
  << ',' << UnCommaStringify(GetCommodities().GetDeck((CommodityType)commType).GetDeckSize())
  << ',' << UnCommaStringify(GetCommodities().GetDeck((CommodityType)commType).GetDiscardSize())
  << ',' << UnCommaStringify(GetCommodities().GetDeck((CommodityType)commType).GetMegaValue())
  << ',' << UnCommaStringify(GetCommodities().GetDeck((CommodityType)commType).GetAverageValue())  ;
  return oss.str();
}

void OutpostGameInfo::UnicastPLAYERFACTORIES(const std::string &i_PlayerName) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    UnicastUpdatePLAYERFACTORIESByplayerindex(i_PlayerName,playerindex);
  }
}
void OutpostGameInfo::VaricastPLAYERFACTORIES(const NameBoolean &i_Variator) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    VaricastUpdatePLAYERFACTORIESByplayerindex(i_Variator,playerindex);
  }
}
void OutpostGameInfo::BroadcastPLAYERFACTORIES() const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    BroadcastUpdatePLAYERFACTORIESByplayerindex(playerindex);
  }
}
void OutpostGameInfo::UnicastUpdatePLAYERFACTORIESByplayerindex(const std::string &i_PlayerName,
      const size_t& playerindex) const
{
  m_OutputPort.UniCast(i_PlayerName,MakePLAYERFACTORIESMessage(playerindex));
}
void OutpostGameInfo::VaricastUpdatePLAYERFACTORIESByplayerindex(const NameBoolean &i_Variator,
      const size_t& playerindex) const
{
  m_OutputPort.VariCast(i_Variator,MakePLAYERFACTORIESMessage(playerindex));
}
void OutpostGameInfo::BroadcastUpdatePLAYERFACTORIESByplayerindex(
      const size_t& playerindex) const
{
  m_OutputPort.BroadCast(MakePLAYERFACTORIESMessage(playerindex));
}

std::string OutpostGameInfo::MakePLAYERFACTORIESMessage(const size_t& playerindex) const
{
  std::ostringstream oss;
  oss << "PLAYERFACTORIES"
  << ',' << UnCommaStringify(playerindex)
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetFactories().GetFactoryDescription())  ;
  return oss.str();
}

void OutpostGameInfo::UnicastPLAYER(const std::string &i_PlayerName) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    UnicastUpdatePLAYERByplayerindex(i_PlayerName,playerindex);
  }
}
void OutpostGameInfo::VaricastPLAYER(const NameBoolean &i_Variator) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    VaricastUpdatePLAYERByplayerindex(i_Variator,playerindex);
  }
}
void OutpostGameInfo::BroadcastPLAYER() const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    BroadcastUpdatePLAYERByplayerindex(playerindex);
  }
}
void OutpostGameInfo::UnicastUpdatePLAYERByplayerindex(const std::string &i_PlayerName,
      const size_t& playerindex) const
{
  m_OutputPort.UniCast(i_PlayerName,MakePLAYERMessage(playerindex));
}
void OutpostGameInfo::VaricastUpdatePLAYERByplayerindex(const NameBoolean &i_Variator,
      const size_t& playerindex) const
{
  m_OutputPort.VariCast(i_Variator,MakePLAYERMessage(playerindex));
}
void OutpostGameInfo::BroadcastUpdatePLAYERByplayerindex(
      const size_t& playerindex) const
{
  m_OutputPort.BroadCast(MakePLAYERMessage(playerindex));
}

std::string OutpostGameInfo::MakePLAYERMessage(const size_t& playerindex) const
{
  std::ostringstream oss;
  oss << "PLAYER"
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetName())
  << ',' << UnCommaStringify(playerindex)  ;
  return oss.str();
}

void OutpostGameInfo::UnicastPLAYERCARDBACKS(const std::string &i_PlayerName) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    UnicastUpdatePLAYERCARDBACKSByplayerindex(i_PlayerName,playerindex);
  }
}
void OutpostGameInfo::VaricastPLAYERCARDBACKS(const NameBoolean &i_Variator) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    VaricastUpdatePLAYERCARDBACKSByplayerindex(i_Variator,playerindex);
  }
}
void OutpostGameInfo::BroadcastPLAYERCARDBACKS() const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    BroadcastUpdatePLAYERCARDBACKSByplayerindex(playerindex);
  }
}
void OutpostGameInfo::UnicastUpdatePLAYERCARDBACKSByplayerindex(const std::string &i_PlayerName,
      const size_t& playerindex) const
{
  m_OutputPort.UniCast(i_PlayerName,MakePLAYERCARDBACKSMessage(playerindex));
}
void OutpostGameInfo::VaricastUpdatePLAYERCARDBACKSByplayerindex(const NameBoolean &i_Variator,
      const size_t& playerindex) const
{
  m_OutputPort.VariCast(i_Variator,MakePLAYERCARDBACKSMessage(playerindex));
}
void OutpostGameInfo::BroadcastUpdatePLAYERCARDBACKSByplayerindex(
      const size_t& playerindex) const
{
  m_OutputPort.BroadCast(MakePLAYERCARDBACKSMessage(playerindex));
}

std::string OutpostGameInfo::MakePLAYERCARDBACKSMessage(const size_t& playerindex) const
{
  std::ostringstream oss;
  oss << "PLAYERCARDBACKS"
  << ',' << UnCommaStringify(playerindex)
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetCommodityHand().GetHandDescription(false,false))  ;
  return oss.str();
}

void OutpostGameInfo::UnicastSPENDS(const std::string &i_PlayerName) const
{
  for( size_t spendIndex = 0 ; spendIndex != GetSpends().size() ; ++spendIndex ) {
    UnicastUpdateSPENDSByspendIndex(i_PlayerName,spendIndex);
  }
}
void OutpostGameInfo::VaricastSPENDS(const NameBoolean &i_Variator) const
{
  for( size_t spendIndex = 0 ; spendIndex != GetSpends().size() ; ++spendIndex ) {
    VaricastUpdateSPENDSByspendIndex(i_Variator,spendIndex);
  }
}
void OutpostGameInfo::BroadcastSPENDS() const
{
  for( size_t spendIndex = 0 ; spendIndex != GetSpends().size() ; ++spendIndex ) {
    BroadcastUpdateSPENDSByspendIndex(spendIndex);
  }
}
void OutpostGameInfo::UnicastUpdateSPENDSByspendIndex(const std::string &i_PlayerName,
      const size_t& spendIndex) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeSPENDSMessage(spendIndex));
}
void OutpostGameInfo::VaricastUpdateSPENDSByspendIndex(const NameBoolean &i_Variator,
      const size_t& spendIndex) const
{
  m_OutputPort.VariCast(i_Variator,MakeSPENDSMessage(spendIndex));
}
void OutpostGameInfo::BroadcastUpdateSPENDSByspendIndex(
      const size_t& spendIndex) const
{
  m_OutputPort.BroadCast(MakeSPENDSMessage(spendIndex));
}

std::string OutpostGameInfo::MakeSPENDSMessage(const size_t& spendIndex) const
{
  std::ostringstream oss;
  oss << "SPENDS"
  << ',' << UnCommaStringify(GetSpends()[spendIndex].GetTurnNum())
  << ',' << UnCommaStringify(GetSpends()[spendIndex].GetPlayerNum())
  << ',' << UnCommaStringify(GetSpends()[spendIndex].GetSpendType())
  << ',' << UnCommaStringify(GetSpends()[spendIndex].GetSpendItem())
  << ',' << UnCommaStringify(GetSpends()[spendIndex].GetSpendCost())
  << ',' << UnCommaStringify(GetSpends()[spendIndex].GetCommodityHand().GetHandDescription(true,true))  ;
  return oss.str();
}

void OutpostGameInfo::UnicastERROR(const std::string &i_PlayerName,const std::string& i_ErrorText) const
{
  ++m_errorcount;
  m_OutputPort.UniCast(i_PlayerName,MakeERRORMessage(i_ErrorText));
}
void OutpostGameInfo::VaricastERROR(const NameBoolean &i_Variator,const std::string& i_ErrorText) const
{
  m_OutputPort.VariCast(i_Variator,MakeERRORMessage(i_ErrorText));
}
void OutpostGameInfo::BroadcastERROR(const std::string& i_ErrorText) const
{
  m_OutputPort.BroadCast(MakeERRORMessage(i_ErrorText));
}

std::string OutpostGameInfo::MakeERRORMessage(const std::string& i_ErrorText) const
{
  std::ostringstream oss;
  oss << "ERROR"
  << ',' << UnCommaStringify(i_ErrorText)  ;
  return oss.str();
}

void OutpostGameInfo::UnicastSUPPLYSHIP(const std::string &i_PlayerName) const
{
  for( size_t shipIndex = 0 ; shipIndex != GetPlayers().size() ; ++shipIndex ) {
    UnicastUpdateSUPPLYSHIPByshipIndex(i_PlayerName,shipIndex);
  }
}
void OutpostGameInfo::VaricastSUPPLYSHIP(const NameBoolean &i_Variator) const
{
  for( size_t shipIndex = 0 ; shipIndex != GetPlayers().size() ; ++shipIndex ) {
    VaricastUpdateSUPPLYSHIPByshipIndex(i_Variator,shipIndex);
  }
}
void OutpostGameInfo::BroadcastSUPPLYSHIP() const
{
  for( size_t shipIndex = 0 ; shipIndex != GetPlayers().size() ; ++shipIndex ) {
    BroadcastUpdateSUPPLYSHIPByshipIndex(shipIndex);
  }
}
void OutpostGameInfo::UnicastUpdateSUPPLYSHIPByshipIndex(const std::string &i_PlayerName,
      const size_t& shipIndex) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeSUPPLYSHIPMessage(shipIndex));
}
void OutpostGameInfo::VaricastUpdateSUPPLYSHIPByshipIndex(const NameBoolean &i_Variator,
      const size_t& shipIndex) const
{
  m_OutputPort.VariCast(i_Variator,MakeSUPPLYSHIPMessage(shipIndex));
}
void OutpostGameInfo::BroadcastUpdateSUPPLYSHIPByshipIndex(
      const size_t& shipIndex) const
{
  m_OutputPort.BroadCast(MakeSUPPLYSHIPMessage(shipIndex));
}

std::string OutpostGameInfo::MakeSUPPLYSHIPMessage(const size_t& shipIndex) const
{
  std::ostringstream oss;
  oss << "SUPPLYSHIP"
  << ',' << UnCommaStringify(shipIndex)
  << ',' << UnCommaStringify(GetUnownedItems().GetShipItem(shipIndex))  ;
  return oss.str();
}

void OutpostGameInfo::UnicastPLAYERWINS(const std::string &i_PlayerName,const size_t& i_PId) const
{
  m_OutputPort.UniCast(i_PlayerName,MakePLAYERWINSMessage(i_PId));
}
void OutpostGameInfo::VaricastPLAYERWINS(const NameBoolean &i_Variator,const size_t& i_PId) const
{
  m_OutputPort.VariCast(i_Variator,MakePLAYERWINSMessage(i_PId));
}
void OutpostGameInfo::BroadcastPLAYERWINS(const size_t& i_PId) const
{
  m_OutputPort.BroadCast(MakePLAYERWINSMessage(i_PId));
}

std::string OutpostGameInfo::MakePLAYERWINSMessage(const size_t& i_PId) const
{
  std::ostringstream oss;
  oss << "PLAYERWINS"
  << ',' << UnCommaStringify(i_PId)  ;
  return oss.str();
}

void OutpostGameInfo::UnicastOPTIONS(const std::string &i_PlayerName) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeOPTIONSMessage());
}
void OutpostGameInfo::VaricastOPTIONS(const NameBoolean &i_Variator) const
{
  m_OutputPort.VariCast(i_Variator,MakeOPTIONSMessage());
}
void OutpostGameInfo::BroadcastOPTIONS() const
{
  m_OutputPort.BroadCast(MakeOPTIONSMessage());
}

std::string OutpostGameInfo::MakeOPTIONSMessage() const
{
  std::ostringstream oss;
  oss << "OPTIONS"
  << ',' << UnCommaStringify(GetOptions().GetEarlyDiscard())
  << ',' << UnCommaStringify(GetOptions().GetRefineries())
  << ',' << UnCommaStringify(GetOptions().GetOneUpgradePerTurn())
  << ',' << UnCommaStringify(GetOptions().GetMustBeActiveToBid())
  << ',' << UnCommaStringify(GetOptions().GetSupplyHarshness())
  << ',' << UnCommaStringify(GetOptions().GetRobotMechanism())
  << ',' << UnCommaStringify(GetOptions().GetResearchMega())
  << ',' << UnCommaStringify(GetOptions().GetResearchIsSmall())
  << ',' << UnCommaStringify(GetOptions().GetMicroIsSmall())
  << ',' << UnCommaStringify(GetOptions().GetBlindDraws())
  << ',' << UnCommaStringify(GetOptions().GetStockMechanism())  ;
  return oss.str();
}

void OutpostGameInfo::UnicastBIDITEM(const std::string &i_PlayerName) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeBIDITEMMessage());
}
void OutpostGameInfo::VaricastBIDITEM(const NameBoolean &i_Variator) const
{
  m_OutputPort.VariCast(i_Variator,MakeBIDITEMMessage());
}
void OutpostGameInfo::BroadcastBIDITEM() const
{
  m_OutputPort.BroadCast(MakeBIDITEMMessage());
}

std::string OutpostGameInfo::MakeBIDITEMMessage() const
{
  std::ostringstream oss;
  oss << "BIDITEM"
  << ',' << UnCommaStringify(GetUnownedItems().GetBidIndex())  ;
  return oss.str();
}

void OutpostGameInfo::SingleCastPLAYERPURCHASEINFO(const size_t& playerindex) const
{
  m_OutputPort.UniCast(GetPlayers()[playerindex].GetName(),MakePLAYERPURCHASEINFOMessage(playerindex));
}
void OutpostGameInfo::AllCastPLAYERPURCHASEINFO() const
{
  for (size_t playerindex=0;playerindex!=GetPlayers().size();++playerindex) {
  SingleCastPLAYERPURCHASEINFO(playerindex);
  }
}

std::string OutpostGameInfo::MakePLAYERPURCHASEINFOMessage(const size_t& playerindex) const
{
  std::ostringstream oss;
  oss << "PLAYERPURCHASEINFO"
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetMaxPurchasable(ROBOT_PURCHASABLE,GetOptions().GetRobotMechanism()))
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetMaxPurchasable(MAN_PURCHASABLE,GetOptions().GetRobotMechanism()))
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetMaxPurchasable(ORE_FACTORY_PURCHASABLE,GetOptions().GetRobotMechanism()))
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetMaxPurchasable(WATER_FACTORY_PURCHASABLE,GetOptions().GetRobotMechanism()))
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetMaxPurchasable(TITANIUM_FACTORY_PURCHASABLE,GetOptions().GetRobotMechanism()))
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetMaxPurchasable(RESEARCH_FACTORY_PURCHASABLE,GetOptions().GetRobotMechanism()))
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetMaxPurchasable(NEW_CHEMICALS_FACTORY_PURCHASABLE,GetOptions().GetRobotMechanism()))  ;
  return oss.str();
}

void OutpostGameInfo::UnicastPLAYERSTATE(const std::string &i_PlayerName) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    UnicastUpdatePLAYERSTATEByplayerindex(i_PlayerName,playerindex);
  }
}
void OutpostGameInfo::VaricastPLAYERSTATE(const NameBoolean &i_Variator) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    VaricastUpdatePLAYERSTATEByplayerindex(i_Variator,playerindex);
  }
}
void OutpostGameInfo::BroadcastPLAYERSTATE() const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    BroadcastUpdatePLAYERSTATEByplayerindex(playerindex);
  }
}
void OutpostGameInfo::UnicastUpdatePLAYERSTATEByplayerindex(const std::string &i_PlayerName,
      const size_t& playerindex) const
{
  m_OutputPort.UniCast(i_PlayerName,MakePLAYERSTATEMessage(playerindex));
}
void OutpostGameInfo::VaricastUpdatePLAYERSTATEByplayerindex(const NameBoolean &i_Variator,
      const size_t& playerindex) const
{
  m_OutputPort.VariCast(i_Variator,MakePLAYERSTATEMessage(playerindex));
}
void OutpostGameInfo::BroadcastUpdatePLAYERSTATEByplayerindex(
      const size_t& playerindex) const
{
  m_OutputPort.BroadCast(MakePLAYERSTATEMessage(playerindex));
}

std::string OutpostGameInfo::MakePLAYERSTATEMessage(const size_t& playerindex) const
{
  std::ostringstream oss;
  oss << "PLAYERSTATE"
  << ',' << UnCommaStringify(playerindex)
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetVP())
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetOwnedItems().GetCostSum())
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetHandLimit(GetOptions().GetEarlyDiscard()))
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetMaxHumans())
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetMaxRobots(GetOptions().GetRobotMechanism()))
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetOwnedItems().GetPeopleCost())
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetNumHumans())
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetNumRobots())
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetPurchaseState())
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetBidState())
  << ',' << UnCommaStringify(GetPlayers()[playerindex].HasPurchased())
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetHighBid())
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetLastBid())  ;
  return oss.str();
}

void OutpostGameInfo::UnicastPLAYEROKSTARTUP(const std::string &i_PlayerName) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    UnicastUpdatePLAYEROKSTARTUPByplayerindex(i_PlayerName,playerindex);
  }
}
void OutpostGameInfo::VaricastPLAYEROKSTARTUP(const NameBoolean &i_Variator) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    VaricastUpdatePLAYEROKSTARTUPByplayerindex(i_Variator,playerindex);
  }
}
void OutpostGameInfo::BroadcastPLAYEROKSTARTUP() const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    BroadcastUpdatePLAYEROKSTARTUPByplayerindex(playerindex);
  }
}
void OutpostGameInfo::UnicastUpdatePLAYEROKSTARTUPByplayerindex(const std::string &i_PlayerName,
      const size_t& playerindex) const
{
  m_OutputPort.UniCast(i_PlayerName,MakePLAYEROKSTARTUPMessage(playerindex));
}
void OutpostGameInfo::VaricastUpdatePLAYEROKSTARTUPByplayerindex(const NameBoolean &i_Variator,
      const size_t& playerindex) const
{
  m_OutputPort.VariCast(i_Variator,MakePLAYEROKSTARTUPMessage(playerindex));
}
void OutpostGameInfo::BroadcastUpdatePLAYEROKSTARTUPByplayerindex(
      const size_t& playerindex) const
{
  m_OutputPort.BroadCast(MakePLAYEROKSTARTUPMessage(playerindex));
}

std::string OutpostGameInfo::MakePLAYEROKSTARTUPMessage(const size_t& playerindex) const
{
  std::ostringstream oss;
  oss << "PLAYEROKSTARTUP"
  << ',' << UnCommaStringify(playerindex)
  << ',' << UnCommaStringify(GetPlayers()[playerindex].OkStartup())  ;
  return oss.str();
}

void OutpostGameInfo::UnicastLEGALACTION(const std::string &i_PlayerName,const std::string& i_ActionName) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeLEGALACTIONMessage(i_ActionName));
}
void OutpostGameInfo::VaricastLEGALACTION(const NameBoolean &i_Variator,const std::string& i_ActionName) const
{
  m_OutputPort.VariCast(i_Variator,MakeLEGALACTIONMessage(i_ActionName));
}
void OutpostGameInfo::BroadcastLEGALACTION(const std::string& i_ActionName) const
{
  m_OutputPort.BroadCast(MakeLEGALACTIONMessage(i_ActionName));
}

std::string OutpostGameInfo::MakeLEGALACTIONMessage(const std::string& i_ActionName) const
{
  std::ostringstream oss;
  oss << "LEGALACTION"
  << ',' << UnCommaStringify(i_ActionName)  ;
  return oss.str();
}

void OutpostGameInfo::UnicastITEMSTOCK(const std::string &i_PlayerName) const
{
  for( int itemType = 1 ; itemType != 14 ; ++itemType ) {
    UnicastUpdateITEMSTOCKByitemType(i_PlayerName,itemType);
  }
}
void OutpostGameInfo::VaricastITEMSTOCK(const NameBoolean &i_Variator) const
{
  for( int itemType = 1 ; itemType != 14 ; ++itemType ) {
    VaricastUpdateITEMSTOCKByitemType(i_Variator,itemType);
  }
}
void OutpostGameInfo::BroadcastITEMSTOCK() const
{
  for( int itemType = 1 ; itemType != 14 ; ++itemType ) {
    BroadcastUpdateITEMSTOCKByitemType(itemType);
  }
}
void OutpostGameInfo::UnicastUpdateITEMSTOCKByitemType(const std::string &i_PlayerName,
      const int& itemType) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeITEMSTOCKMessage(itemType));
}
void OutpostGameInfo::VaricastUpdateITEMSTOCKByitemType(const NameBoolean &i_Variator,
      const int& itemType) const
{
  m_OutputPort.VariCast(i_Variator,MakeITEMSTOCKMessage(itemType));
}
void OutpostGameInfo::BroadcastUpdateITEMSTOCKByitemType(
      const int& itemType) const
{
  m_OutputPort.BroadCast(MakeITEMSTOCKMessage(itemType));
}

std::string OutpostGameInfo::MakeITEMSTOCKMessage(const int& itemType) const
{
  std::ostringstream oss;
  oss << "ITEMSTOCK"
  << ',' << UnCommaStringify(itemType)
  << ',' << UnCommaStringify(GetUnownedItems().GetItemCount((ItemType)itemType))  ;
  return oss.str();
}

void OutpostGameInfo::UnicastMESSAGE(const std::string &i_PlayerName,const std::string& i_MessageText) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeMESSAGEMessage(i_MessageText));
}
void OutpostGameInfo::VaricastMESSAGE(const NameBoolean &i_Variator,const std::string& i_MessageText) const
{
  m_OutputPort.VariCast(i_Variator,MakeMESSAGEMessage(i_MessageText));
}
void OutpostGameInfo::BroadcastMESSAGE(const std::string& i_MessageText) const
{
  m_OutputPort.BroadCast(MakeMESSAGEMessage(i_MessageText));
}

std::string OutpostGameInfo::MakeMESSAGEMessage(const std::string& i_MessageText) const
{
  std::ostringstream oss;
  oss << "MESSAGE"
  << ',' << UnCommaStringify(i_MessageText)  ;
  return oss.str();
}

void OutpostGameInfo::UnicastPURCHASEPRICE(const std::string &i_PlayerName) const
{
  m_OutputPort.UniCast(i_PlayerName,MakePURCHASEPRICEMessage());
}
void OutpostGameInfo::VaricastPURCHASEPRICE(const NameBoolean &i_Variator) const
{
  m_OutputPort.VariCast(i_Variator,MakePURCHASEPRICEMessage());
}
void OutpostGameInfo::BroadcastPURCHASEPRICE() const
{
  m_OutputPort.BroadCast(MakePURCHASEPRICEMessage());
}

std::string OutpostGameInfo::MakePURCHASEPRICEMessage() const
{
  std::ostringstream oss;
  oss << "PURCHASEPRICE"
  << ',' << UnCommaStringify(GetPlayers().GetHighBidPlayer().GetHighBid() -         GetPlayers().GetHighBidPlayer().GetOwnedItems().GetDiscount(GetUnownedItems().GetShipItem(GetUnownedItems().GetBidIndex())))  ;
  return oss.str();
}

void OutpostGameInfo::UnicastPLAYERITEMS(const std::string &i_PlayerName) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    UnicastUpdatePLAYERITEMSByplayerindex(i_PlayerName,playerindex);
  }
}
void OutpostGameInfo::VaricastPLAYERITEMS(const NameBoolean &i_Variator) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    VaricastUpdatePLAYERITEMSByplayerindex(i_Variator,playerindex);
  }
}
void OutpostGameInfo::BroadcastPLAYERITEMS() const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    BroadcastUpdatePLAYERITEMSByplayerindex(playerindex);
  }
}
void OutpostGameInfo::UnicastUpdatePLAYERITEMSByplayerindex(const std::string &i_PlayerName,
      const size_t& playerindex) const
{
  for( size_t itemindex = 0 ; itemindex != GetPlayers()[playerindex].GetOwnedItems().GetItemList().size() ; ++itemindex ) {
    UnicastUpdatePLAYERITEMSByplayerindexAndByitemindex(i_PlayerName,playerindex,itemindex);
  }
}
void OutpostGameInfo::VaricastUpdatePLAYERITEMSByplayerindex(const NameBoolean &i_Variator,
      const size_t& playerindex) const
{
  for( size_t itemindex = 0 ; itemindex != GetPlayers()[playerindex].GetOwnedItems().GetItemList().size() ; ++itemindex ) {
    VaricastUpdatePLAYERITEMSByplayerindexAndByitemindex(i_Variator,playerindex,itemindex);
  }
}
void OutpostGameInfo::BroadcastUpdatePLAYERITEMSByplayerindex(
      const size_t& playerindex) const
{
  for( size_t itemindex = 0 ; itemindex != GetPlayers()[playerindex].GetOwnedItems().GetItemList().size() ; ++itemindex ) {
    BroadcastUpdatePLAYERITEMSByplayerindexAndByitemindex(playerindex,itemindex);
  }
}
void OutpostGameInfo::UnicastUpdatePLAYERITEMSByplayerindexAndByitemindex(const std::string &i_PlayerName,
      const size_t& playerindex,const size_t& itemindex) const
{
  m_OutputPort.UniCast(i_PlayerName,MakePLAYERITEMSMessage(playerindex,itemindex));
}
void OutpostGameInfo::VaricastUpdatePLAYERITEMSByplayerindexAndByitemindex(const NameBoolean &i_Variator,
      const size_t& playerindex,const size_t& itemindex) const
{
  m_OutputPort.VariCast(i_Variator,MakePLAYERITEMSMessage(playerindex,itemindex));
}
void OutpostGameInfo::BroadcastUpdatePLAYERITEMSByplayerindexAndByitemindex(
      const size_t& playerindex,const size_t& itemindex) const
{
  m_OutputPort.BroadCast(MakePLAYERITEMSMessage(playerindex,itemindex));
}

std::string OutpostGameInfo::MakePLAYERITEMSMessage(const size_t& playerindex,const size_t& itemindex) const
{
  std::ostringstream oss;
  oss << "PLAYERITEMS"
  << ',' << UnCommaStringify(playerindex)
  << ',' << UnCommaStringify(itemindex)
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetOwnedItems().GetItemList()[itemindex])  ;
  return oss.str();
}

void OutpostGameInfo::SingleCastPLAYERCARDS(const size_t& playerindex) const
{
  m_OutputPort.UniCast(GetPlayers()[playerindex].GetName(),MakePLAYERCARDSMessage(playerindex));
}
void OutpostGameInfo::AllCastPLAYERCARDS() const
{
  for (size_t playerindex=0;playerindex!=GetPlayers().size();++playerindex) {
  SingleCastPLAYERCARDS(playerindex);
  }
}

std::string OutpostGameInfo::MakePLAYERCARDSMessage(const size_t& playerindex) const
{
  std::ostringstream oss;
  oss << "PLAYERCARDS"
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetCommodityHand().GetHandDescription(true,                                                         GetProductionManager().IsProductionDone() || !GetOptions().GetBlindDraws() ))  ;
  return oss.str();
}

void OutpostGameInfo::UnicastMEGAREQUEST(const std::string &i_PlayerName) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeMEGAREQUESTMessage());
}
void OutpostGameInfo::VaricastMEGAREQUEST(const NameBoolean &i_Variator) const
{
  m_OutputPort.VariCast(i_Variator,MakeMEGAREQUESTMessage());
}
void OutpostGameInfo::BroadcastMEGAREQUEST() const
{
  m_OutputPort.BroadCast(MakeMEGAREQUESTMessage());
}

std::string OutpostGameInfo::MakeMEGAREQUESTMessage() const
{
  std::ostringstream oss;
  oss << "MEGAREQUEST"
  << ',' << UnCommaStringify(GetProductionManager().GetMegaInfo().m_Commodity)
  << ',' << UnCommaStringify(GetProductionManager().GetMegaInfo().m_MaxMegas)  ;
  return oss.str();
}

void OutpostGameInfo::UnicastCURTURN(const std::string &i_PlayerName) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeCURTURNMessage());
}
void OutpostGameInfo::VaricastCURTURN(const NameBoolean &i_Variator) const
{
  m_OutputPort.VariCast(i_Variator,MakeCURTURNMessage());
}
void OutpostGameInfo::BroadcastCURTURN() const
{
  m_OutputPort.BroadCast(MakeCURTURNMessage());
}

std::string OutpostGameInfo::MakeCURTURNMessage() const
{
  std::ostringstream oss;
  oss << "CURTURN"
  << ',' << UnCommaStringify(GetCurTurn())  ;
  return oss.str();
}

void OutpostGameInfo::UnicastTURNORDER(const std::string &i_PlayerName) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    UnicastUpdateTURNORDERByplayerindex(i_PlayerName,playerindex);
  }
}
void OutpostGameInfo::VaricastTURNORDER(const NameBoolean &i_Variator) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    VaricastUpdateTURNORDERByplayerindex(i_Variator,playerindex);
  }
}
void OutpostGameInfo::BroadcastTURNORDER() const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    BroadcastUpdateTURNORDERByplayerindex(playerindex);
  }
}
void OutpostGameInfo::UnicastUpdateTURNORDERByplayerindex(const std::string &i_PlayerName,
      const size_t& playerindex) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeTURNORDERMessage(playerindex));
}
void OutpostGameInfo::VaricastUpdateTURNORDERByplayerindex(const NameBoolean &i_Variator,
      const size_t& playerindex) const
{
  m_OutputPort.VariCast(i_Variator,MakeTURNORDERMessage(playerindex));
}
void OutpostGameInfo::BroadcastUpdateTURNORDERByplayerindex(
      const size_t& playerindex) const
{
  m_OutputPort.BroadCast(MakeTURNORDERMessage(playerindex));
}

std::string OutpostGameInfo::MakeTURNORDERMessage(const size_t& playerindex) const
{
  std::ostringstream oss;
  oss << "TURNORDER"
  << ',' << UnCommaStringify(GetPlayers().GetTurnOrder()[playerindex])
  << ',' << UnCommaStringify(playerindex+1)  ;
  return oss.str();
}

void OutpostGameInfo::SingleCastIAM(const size_t& playerindex) const
{
  m_OutputPort.UniCast(GetPlayers()[playerindex].GetName(),MakeIAMMessage(playerindex));
}
void OutpostGameInfo::AllCastIAM() const
{
  for (size_t playerindex=0;playerindex!=GetPlayers().size();++playerindex) {
  SingleCastIAM(playerindex);
  }
}

std::string OutpostGameInfo::MakeIAMMessage(const size_t& playerindex) const
{
  std::ostringstream oss;
  oss << "IAM"
  << ',' << UnCommaStringify(playerindex)  ;
  return oss.str();
}

void OutpostGameInfo::UnicastRESET(const std::string &i_PlayerName) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeRESETMessage());
}
void OutpostGameInfo::VaricastRESET(const NameBoolean &i_Variator) const
{
  m_OutputPort.VariCast(i_Variator,MakeRESETMessage());
}
void OutpostGameInfo::BroadcastRESET() const
{
  m_OutputPort.BroadCast(MakeRESETMessage());
}

std::string OutpostGameInfo::MakeRESETMessage() const
{
  std::ostringstream oss;
  oss << "RESET"
  << ',' << UnCommaStringify(m_ServerGameInfo.GetName())  ;
  return oss.str();
}

void OutpostGameInfo::UnicastNEWSTATE(const std::string &i_PlayerName,const std::string& i_EventName,const std::string& i_EventDesc) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeNEWSTATEMessage(i_EventName,i_EventDesc));
}
void OutpostGameInfo::VaricastNEWSTATE(const NameBoolean &i_Variator,const std::string& i_EventName,const std::string& i_EventDesc) const
{
  m_OutputPort.VariCast(i_Variator,MakeNEWSTATEMessage(i_EventName,i_EventDesc));
}
void OutpostGameInfo::BroadcastNEWSTATE(const std::string& i_EventName,const std::string& i_EventDesc) const
{
  m_OutputPort.BroadCast(MakeNEWSTATEMessage(i_EventName,i_EventDesc));
}

std::string OutpostGameInfo::MakeNEWSTATEMessage(const std::string& i_EventName,const std::string& i_EventDesc) const
{
  std::ostringstream oss;
  oss << "NEWSTATE"
  << ',' << UnCommaStringify(i_EventName)
  << ',' << UnCommaStringify(i_EventDesc)  ;
  return oss.str();
}

void OutpostGameInfo::UnicastPURCHASEDATA(const std::string &i_PlayerName) const
{
  for( size_t shipIndex = 0 ; shipIndex != GetPlayers().size() ; ++shipIndex ) {
    UnicastUpdatePURCHASEDATAByshipIndex(i_PlayerName,shipIndex);
  }
}
void OutpostGameInfo::VaricastPURCHASEDATA(const NameBoolean &i_Variator) const
{
  for( size_t shipIndex = 0 ; shipIndex != GetPlayers().size() ; ++shipIndex ) {
    VaricastUpdatePURCHASEDATAByshipIndex(i_Variator,shipIndex);
  }
}
void OutpostGameInfo::BroadcastPURCHASEDATA() const
{
  for( size_t shipIndex = 0 ; shipIndex != GetPlayers().size() ; ++shipIndex ) {
    BroadcastUpdatePURCHASEDATAByshipIndex(shipIndex);
  }
}
void OutpostGameInfo::UnicastUpdatePURCHASEDATAByshipIndex(const std::string &i_PlayerName,
      const size_t& shipIndex) const
{
  m_OutputPort.UniCast(i_PlayerName,MakePURCHASEDATAMessage(shipIndex));
}
void OutpostGameInfo::VaricastUpdatePURCHASEDATAByshipIndex(const NameBoolean &i_Variator,
      const size_t& shipIndex) const
{
  m_OutputPort.VariCast(i_Variator,MakePURCHASEDATAMessage(shipIndex));
}
void OutpostGameInfo::BroadcastUpdatePURCHASEDATAByshipIndex(
      const size_t& shipIndex) const
{
  m_OutputPort.BroadCast(MakePURCHASEDATAMessage(shipIndex));
}

std::string OutpostGameInfo::MakePURCHASEDATAMessage(const size_t& shipIndex) const
{
  std::ostringstream oss;
  oss << "PURCHASEDATA"
  << ',' << UnCommaStringify(shipIndex)
  << ',' << UnCommaStringify(GetUnownedItems().GetPurchaseInfo(shipIndex).m_playerindex)
  << ',' << UnCommaStringify(GetUnownedItems().GetPurchaseInfo(shipIndex).m_type)
  << ',' << UnCommaStringify(GetUnownedItems().GetPurchaseInfo(shipIndex).m_cost)  ;
  return oss.str();
}


OutpostGameInfo::OutpostGameInfo(const OutpostSet &i_Set,
                                 const ServerGameInfo &i_ginfo,
                                 OutputPort  &i_Port) :
  GameInfo<OutpostSet,OutpostState>(i_Set,i_ginfo,i_Port) {}

void OutpostGameInfo::ResetERRORCount() { m_errorcount = 0; }
int OutpostGameInfo::GetERRORCount() const { return m_errorcount; }
bool OutpostGameInfo::PASS_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "PASS";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().GetCurBidPlayer().GetName() == i_PlayerName &&
	GetPlayers().GetCurBidPlayer().GetHighBid() == NOT_HIGH_BID
      
    ;
}

bool OutpostGameInfo::PASS_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "PASS";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::PASS_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "PASS";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to PASS");
    return false;
  }

  
      
        size_t oldindex = GetPlayers().GetCurBidPlayer().GetId();

        GetPlayers().IncrementBid(PASS);

	BroadcastUpdatePLAYERSTATEByplayerindex(oldindex);
	BroadcastUpdatePLAYERSTATEByplayerindex(GetPlayers().GetCurBidPlayer().GetId());
        return true;
      
    ;
}

bool OutpostGameInfo::FILLSHIP_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::SupplyFill
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "FILLSHIP";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::FILLSHIP_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::SupplyFill
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "FILLSHIP";
    throw std::out_of_range(oss.str());
  }

  return true;
}

bool OutpostGameInfo::FILLSHIP_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::SupplyFill
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "FILLSHIP";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to FILLSHIP");
    return false;
  }

  
      
        GetUnownedItems().FillShip(GetPlayers().GetPhase(),GetOptions().GetSupplyHarshness());
	GetUnownedItems().ClearPurchases();

	BroadcastSUPPLYSHIP();
	BroadcastITEMSTOCK();
	BroadcastPURCHASEDATA();


        return true;
      
    ;
}

bool OutpostGameInfo::JOIN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "JOIN";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().size() < MAXNUMPLAYERS && !GetPlayers().IsPlayer(i_PlayerName)
      
    ;
}

bool OutpostGameInfo::JOIN_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "JOIN";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::JOIN_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "JOIN";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to JOIN");
    return false;
  }

  
      GetPlayers().add(i_PlayerName);
      BroadcastUpdatePLAYERByplayerindex(GetPlayers().size()-1);
      SingleCastIAM(GetPlayers().size()-1);
      return true;
    ;
}

bool OutpostGameInfo::OPENBID_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "OPENBID";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().IsPlayer(i_PlayerName) &&
	GetPlayers().GetPlayerByName(i_PlayerName).GetPurchaseState() == ACTIVE &&
	GetUnownedItems().GetNonEmptyShipIndices().size() > 0 &&
	(!GetOptions().GetOneUpgradePerTurn() || !GetPlayers().GetPlayerByName(i_PlayerName).HasPurchased()) &&
	GetUnownedItems().CanPlayerAffordItem(GetPlayers().GetPlayerByName(i_PlayerName),ANY_SHIP_SLOT)
      
    ;
}

bool OutpostGameInfo::OPENBID_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "OPENBID";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::OPENBID_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::Purchase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "OPENBID";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 2)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to OPENBID");
    return false;
  }

  size_t i_shipslot(boost::lexical_cast<size_t>(i_ap[0]));

  std::set<size_t> validlist(GetUnownedItems().GetNonEmptyShipIndices());
  std::set<size_t>::iterator findit = validlist.find(i_shipslot);
  if (findit == validlist.end())
  {
    UnicastERROR(i_PlayerName,"Action OPENBID variable i_shipslot has illegal value");
    return false;
  }

  int i_openingbid(boost::lexical_cast<int>(i_ap[1]));

  if (i_openingbid < GetUnownedItems().GetCost(GetUnownedItems().GetShipItem(i_shipslot)))
  {
    UnicastERROR(i_PlayerName,"Action OPENBID variable i_openingbid is below minimum");
    return false;
  }

  if (i_openingbid > GetPlayers().GetPlayerByName(i_PlayerName).GetCommodityHand().GetHandValue() +          GetPlayers().GetPlayerByName(i_PlayerName).GetOwnedItems().GetDiscount(GetUnownedItems().GetShipItem(i_shipslot)))
  {
    UnicastERROR(i_PlayerName,"Action OPENBID variable i_openingbid is above maximum");
    return false;
  }

  
      
        GetPlayers().StartBidPhase(GetOptions().GetMustBeActiveToBid(),GetOptions().GetOneUpgradePerTurn(),i_openingbid);
	GetUnownedItems().SetBidIndex(i_shipslot);

	size_t i;
	for (i = 0; i < GetPlayers().size() ; ++i)
	{	
	  GetPlayers()[i].SetLastBid(NOT_HIGH_BID);
	  if (GetPlayers()[i].GetHighBid() != NOT_HIGH_BID)
	  {
	    GetPlayers()[i].SetLastBid(GetPlayers()[i].GetHighBid());
	  }
	}

	BroadcastPLAYERSTATE();
	BroadcastBIDITEM();
	BroadcastPURCHASEPRICE();

	return true;
      
    ;
}

bool OutpostGameInfo::BUYOREFACTORIES_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYOREFACTORIES";
    throw std::out_of_range(oss.str());
  }

  return  
      
        GetPlayers().CanPlayerPurchase(i_PlayerName,ORE_FACTORY_PURCHASABLE,GetOptions().GetRobotMechanism())
      
    ;
}

bool OutpostGameInfo::BUYOREFACTORIES_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYOREFACTORIES";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::BUYOREFACTORIES_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYOREFACTORIES";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 2)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to BUYOREFACTORIES");
    return false;
  }

  int i_numfactories(boost::lexical_cast<int>(i_ap[0]));

  if (i_numfactories < 0)
  {
    UnicastERROR(i_PlayerName,"Action BUYOREFACTORIES variable i_numfactories is below minimum");
    return false;
  }

  if (i_numfactories > GetPlayers().GetPlayerByName(i_PlayerName).GetMaxPurchasable(ORE_FACTORY_PURCHASABLE,GetOptions().GetRobotMechanism()))
  {
    UnicastERROR(i_PlayerName,"Action BUYOREFACTORIES variable i_numfactories is above maximum");
    return false;
  }

  std::string i_discardstring(boost::lexical_cast<std::string>(i_ap[1]));

  
      
        Spend spend(GetCurTurn(),
	            GetPlayers().GetPlayerByName(i_PlayerName).GetId(),
		    ITEMPURCHASE,
		    ORE_FACTORY_PURCHASABLE
		    );

        HandDeleter hd(*this,GetPlayers().GetPlayerByName(i_PlayerName),i_discardstring,spend,false,i_numfactories * 10);
	if (!hd.IsOk()) return false;

	GetSpends().push_back(spend);
        BroadcastUpdateSPENDSByspendIndex(GetSpends().size()-1);

	for (int i = 0 ; i < i_numfactories ; ++i)
	{
	  GetPlayers().GetPlayerByName(i_PlayerName).GetFactories().AddFactory(ORE_FACTORY);
	}

	GetPlayers().GetPlayerByName(i_PlayerName).AutoMan();

	BroadcastUpdatePLAYERSTATEByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	BroadcastUpdatePLAYERFACTORIESByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	BroadcastUpdatePLAYERCARDBACKSByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	SingleCastPLAYERCARDS(GetPlayers().GetPlayerId(i_PlayerName) );
	SingleCastPLAYERPURCHASEINFO(GetPlayers().GetPlayerId(i_PlayerName));
	BroadcastCOMMODITYSTOCK();
        return true;
      
    ;
}

bool OutpostGameInfo::DONTLIKEOPTIONS_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::ValidateSetup
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "DONTLIKEOPTIONS";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().IsPlayer(i_PlayerName) && 
        !GetPlayers().GetPlayerByName(i_PlayerName).OkStartup()
      
    ;
}

bool OutpostGameInfo::DONTLIKEOPTIONS_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::ValidateSetup
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "DONTLIKEOPTIONS";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::DONTLIKEOPTIONS_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::ValidateSetup
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "DONTLIKEOPTIONS";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to DONTLIKEOPTIONS");
    return false;
  }

  return true;;
}

bool OutpostGameInfo::DISCARD_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::ManualDiscard
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "DISCARD";
    throw std::out_of_range(oss.str());
  }

  return  i_PlayerName == GetPlayers().GetCurTurnPlayer().GetName() ;
}

bool OutpostGameInfo::DISCARD_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::ManualDiscard
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "DISCARD";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::DISCARD_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::ManualDiscard
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "DISCARD";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 1)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to DISCARD");
    return false;
  }

  std::string i_discardstring(boost::lexical_cast<std::string>(i_ap[0]));

  
     
      Spend spend(GetCurTurn(),
                  GetPlayers().GetCurTurnPlayer().GetId(),
		  DISCARD,
		  0
		  );
		  
      HandDeleter hd(*this,GetPlayers().GetCurTurnPlayer(),i_discardstring,spend,true);
      if (!hd.IsOk()) return false;

      GetSpends().push_back(spend);
      BroadcastUpdateSPENDSByspendIndex(GetSpends().size()-1);

      BroadcastUpdatePLAYERCARDBACKSByplayerindex(GetPlayers().GetCurTurnPlayer().GetId());
      SingleCastPLAYERCARDS(GetPlayers().GetCurTurnPlayer().GetId());

      size_t oldid = GetPlayers().GetCurTurnPlayer().GetId();
      GetPlayers().IncrementTurnOrder();
      BroadcastUpdatePLAYERSTATEByplayerindex(oldid);
      if (!GetPlayers().AllPlayersDone()) BroadcastUpdatePLAYERSTATEByplayerindex(GetPlayers().GetCurTurnPlayer().GetId());

      BroadcastCOMMODITYSTOCK();

      return true;
     
    ;
}

bool OutpostGameInfo::PURCHASEBID_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "PURCHASEBID";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().GetCurBidPlayer().GetName() == i_PlayerName &&
	GetPlayers().GetCurBidPlayer().GetHighBid() != NOT_HIGH_BID
      
    ;
}

bool OutpostGameInfo::PURCHASEBID_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "PURCHASEBID";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::PURCHASEBID_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "PURCHASEBID";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 1)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to PURCHASEBID");
    return false;
  }

  std::string i_discardstring(boost::lexical_cast<std::string>(i_ap[0]));

  
      
        Spend spend(GetCurTurn(),
                    GetPlayers().GetCurBidPlayer().GetId(),
		    BIDPURCHASE,
		    GetUnownedItems().GetShipItem(GetUnownedItems().GetBidIndex())
		    );

        HandDeleter hd(*this,GetPlayers().GetCurBidPlayer(),
	               i_discardstring,spend,false,GetPlayers().GetCurBidPlayer().GetHighBid() - 
		                                   GetPlayers().GetCurBidPlayer().GetOwnedItems().GetDiscount(
					             GetUnownedItems().GetShipItem(GetUnownedItems().GetBidIndex())),0);
	if (!hd.IsOk()) return false;

	GetSpends().push_back(spend);
        BroadcastUpdateSPENDSByspendIndex(GetSpends().size()-1);

	Player &pl = GetPlayers().GetCurBidPlayer();
	size_t itemindex = GetUnownedItems().GetBidIndex();
	ItemType curitem = GetUnownedItems().GetShipItem(itemindex);

	int finalcost = pl.GetHighBid() - pl.GetOwnedItems().GetDiscount(curitem);

	FactoryType newfac = pl.GetOwnedItems().AddItem(GetUnownedItems().GetShipItem(GetUnownedItems().GetBidIndex()));
	if (newfac != NO_FACTORY) pl.GetFactories().AddFactory(newfac);

	pl.SetHighBid(NOT_HIGH_BID);
	pl.SetHasPurchased(true);
	pl.SetBidState(PASS_OUT);


	GetUnownedItems().SetPurchased(itemindex,curitem,pl.GetId(),finalcost);
	GetUnownedItems().SetShipItem(GetUnownedItems().GetBidIndex(),NO_ITEM);
	pl.AutoMan();

	BroadcastUpdateSUPPLYSHIPByshipIndex(GetUnownedItems().GetBidIndex());

	GetUnownedItems().SetBidIndex(NO_BID_INDEX);

	BroadcastUpdatePLAYERCARDBACKSByplayerindex(pl.GetId());
	SingleCastPLAYERCARDS( pl.GetId() );
	SingleCastPLAYERPURCHASEINFO( pl.GetId() );
	BroadcastUpdatePLAYERITEMSByplayerindex(pl.GetId());
	BroadcastUpdatePLAYERFACTORIESByplayerindex(pl.GetId());
	BroadcastUpdatePLAYERSTATEByplayerindex(pl.GetId());
	BroadcastBIDITEM();
	BroadcastCOMMODITYSTOCK();
	BroadcastUpdatePURCHASEDATAByshipIndex(itemindex);

        return true;
      
    ;
}

bool OutpostGameInfo::AUTOSKIPDISCARD_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::DoDiscard
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "AUTOSKIPDISCARD";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::AUTOSKIPDISCARD_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::DoDiscard
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "AUTOSKIPDISCARD";
    throw std::out_of_range(oss.str());
  }

  return 
      
        !GetPlayers().AllPlayersDone() &&
         GetPlayers().GetCurTurnPlayer().GetHandLimit(GetOptions().GetEarlyDiscard()) >=
         GetPlayers().GetCurTurnPlayer().GetCommodityHand().GetHandWeight(GetOptions().GetResearchIsSmall(),
      						                          GetOptions().GetMicroIsSmall())
      
    ;
}

bool OutpostGameInfo::AUTOSKIPDISCARD_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::DoDiscard
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "AUTOSKIPDISCARD";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to AUTOSKIPDISCARD");
    return false;
  }

  
      GetPlayers().IncrementTurnOrder();
      BroadcastPLAYERSTATE();
      return true;
    ;
}

bool OutpostGameInfo::BUYTITANIUMFACTORIES_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYTITANIUMFACTORIES";
    throw std::out_of_range(oss.str());
  }

  return  
      
        GetPlayers().CanPlayerPurchase(i_PlayerName,TITANIUM_FACTORY_PURCHASABLE,GetOptions().GetRobotMechanism())
      
    ;
}

bool OutpostGameInfo::BUYTITANIUMFACTORIES_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYTITANIUMFACTORIES";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::BUYTITANIUMFACTORIES_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYTITANIUMFACTORIES";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 2)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to BUYTITANIUMFACTORIES");
    return false;
  }

  int i_numfactories(boost::lexical_cast<int>(i_ap[0]));

  if (i_numfactories < 0)
  {
    UnicastERROR(i_PlayerName,"Action BUYTITANIUMFACTORIES variable i_numfactories is below minimum");
    return false;
  }

  if (i_numfactories > GetPlayers().GetPlayerByName(i_PlayerName).GetMaxPurchasable(TITANIUM_FACTORY_PURCHASABLE,GetOptions().GetRobotMechanism()))
  {
    UnicastERROR(i_PlayerName,"Action BUYTITANIUMFACTORIES variable i_numfactories is above maximum");
    return false;
  }

  std::string i_discardstring(boost::lexical_cast<std::string>(i_ap[1]));

  
      
        Spend spend(GetCurTurn(),
	            GetPlayers().GetPlayerByName(i_PlayerName).GetId(),
		    ITEMPURCHASE,
		    TITANIUM_FACTORY_PURCHASABLE
		    );

        HandDeleter hd(*this,GetPlayers().GetPlayerByName(i_PlayerName),i_discardstring,spend,false,i_numfactories * 30);
	if (!hd.IsOk()) return false;

	GetSpends().push_back(spend);
        BroadcastUpdateSPENDSByspendIndex(GetSpends().size()-1);

	for (int i = 0 ; i < i_numfactories ; ++i)
	{
	  GetPlayers().GetPlayerByName(i_PlayerName).GetFactories().AddFactory(TITANIUM_FACTORY);
	}

	GetPlayers().GetPlayerByName(i_PlayerName).AutoMan();

	BroadcastUpdatePLAYERSTATEByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	BroadcastUpdatePLAYERFACTORIESByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	BroadcastUpdatePLAYERCARDBACKSByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	SingleCastPLAYERCARDS(GetPlayers().GetPlayerId(i_PlayerName) );
	SingleCastPLAYERPURCHASEINFO(GetPlayers().GetPlayerId(i_PlayerName));
	BroadcastCOMMODITYSTOCK();
        return true;
      
    ;
}

bool OutpostGameInfo::DETERMINETURNORDER_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::TurnOrder
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "DETERMINETURNORDER";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::DETERMINETURNORDER_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::TurnOrder
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "DETERMINETURNORDER";
    throw std::out_of_range(oss.str());
  }

  return true;
}

bool OutpostGameInfo::DETERMINETURNORDER_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::TurnOrder
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "DETERMINETURNORDER";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to DETERMINETURNORDER");
    return false;
  }

  
      
        GetPlayers().DetermineTurnOrder();
	BroadcastTURNORDER();
        return true;
      
    ;
}

bool OutpostGameInfo::DONEMANNING_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::FinalManning
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "DONEMANNING";
    throw std::out_of_range(oss.str());
  }

  return 
      
        !GetPlayers().AllPlayersDone() &&
        GetPlayers().GetCurTurnPlayer().GetName() == i_PlayerName &&
	GetPlayers().GetCurTurnPlayer().GetMaxHumans() >= GetPlayers().GetCurTurnPlayer().GetFactories().GetUsedMen() &&
	(GetPlayers().GetCurTurnPlayer().GetMaxRobots(GetOptions().GetRobotMechanism()) == UNLIMITED_ROBOTS ||
	GetPlayers().GetCurTurnPlayer().GetMaxRobots(GetOptions().GetRobotMechanism()) >= 
	    GetPlayers().GetCurTurnPlayer().GetFactories().GetUsedRobots())
      
    ;
}

bool OutpostGameInfo::DONEMANNING_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::FinalManning
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "DONEMANNING";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::DONEMANNING_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::FinalManning
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "DONEMANNING";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to DONEMANNING");
    return false;
  }

  
      
      Player &oldpl = GetPlayers().GetCurTurnPlayer();

      if (oldpl.GetMaxHumans() < oldpl.GetNumHumans())
      {
        oldpl.AddHumans( -1 * (oldpl.GetNumHumans() - oldpl.GetMaxHumans() ) );
      }

      if (oldpl.GetMaxRobots(GetOptions().GetRobotMechanism()) != UNLIMITED_ROBOTS &&
          oldpl.GetMaxRobots(GetOptions().GetRobotMechanism()) < oldpl.GetNumRobots())
      {
        oldpl.AddRobots( -1 * (oldpl.GetNumRobots() - oldpl.GetMaxRobots(GetOptions().GetRobotMechanism()) ) );
      }

      GetPlayers().IncrementManningTurnOrder();
        
      BroadcastUpdatePLAYERSTATEByplayerindex(oldpl.GetId());
      if (!GetPlayers().AllPlayersDone())
      {
        BroadcastUpdatePLAYERSTATEByplayerindex(GetPlayers().GetCurTurnPlayer().GetId());
      }
      return true;
      
    ;
}

bool OutpostGameInfo::VALIDATESETUP_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "VALIDATESETUP";
    throw std::out_of_range(oss.str());
  }

  return 
      
         GetPlayers().IsPlayer(i_PlayerName) && GetPlayers().size() > 1
      
    ;
}

bool OutpostGameInfo::VALIDATESETUP_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "VALIDATESETUP";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::VALIDATESETUP_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "VALIDATESETUP";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to VALIDATESETUP");
    return false;
  }

  
      
        size_t i;
        for (i = 0 ; i < GetPlayers().size() ; ++i)
	{
	  GetPlayers()[i].SetOkStartup(false);
	}
	
	BroadcastPLAYEROKSTARTUP();

	return true;
      
    ;
}

bool OutpostGameInfo::BUYWATERFACTORIES_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYWATERFACTORIES";
    throw std::out_of_range(oss.str());
  }

  return  
      
        GetPlayers().CanPlayerPurchase(i_PlayerName,WATER_FACTORY_PURCHASABLE,GetOptions().GetRobotMechanism())
      
    ;
}

bool OutpostGameInfo::BUYWATERFACTORIES_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYWATERFACTORIES";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::BUYWATERFACTORIES_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYWATERFACTORIES";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 2)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to BUYWATERFACTORIES");
    return false;
  }

  int i_numfactories(boost::lexical_cast<int>(i_ap[0]));

  if (i_numfactories < 0)
  {
    UnicastERROR(i_PlayerName,"Action BUYWATERFACTORIES variable i_numfactories is below minimum");
    return false;
  }

  if (i_numfactories > GetPlayers().GetPlayerByName(i_PlayerName).GetMaxPurchasable(WATER_FACTORY_PURCHASABLE,GetOptions().GetRobotMechanism()))
  {
    UnicastERROR(i_PlayerName,"Action BUYWATERFACTORIES variable i_numfactories is above maximum");
    return false;
  }

  std::string i_discardstring(boost::lexical_cast<std::string>(i_ap[1]));

  
      
        Spend spend(GetCurTurn(),
	            GetPlayers().GetPlayerByName(i_PlayerName).GetId(),
		    ITEMPURCHASE,
		    WATER_FACTORY_PURCHASABLE
		    );

        HandDeleter hd(*this,GetPlayers().GetPlayerByName(i_PlayerName),i_discardstring,spend,false,i_numfactories * 20);
	if (!hd.IsOk()) return false;

	GetSpends().push_back(spend);
        BroadcastUpdateSPENDSByspendIndex(GetSpends().size()-1);

	for (int i = 0 ; i < i_numfactories ; ++i)
	{
	  GetPlayers().GetPlayerByName(i_PlayerName).GetFactories().AddFactory(WATER_FACTORY);
	}

	GetPlayers().GetPlayerByName(i_PlayerName).AutoMan();

	BroadcastUpdatePLAYERSTATEByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	BroadcastUpdatePLAYERFACTORIESByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	BroadcastUpdatePLAYERCARDBACKSByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	SingleCastPLAYERCARDS(GetPlayers().GetPlayerId(i_PlayerName) );
	SingleCastPLAYERPURCHASEINFO(GetPlayers().GetPlayerId(i_PlayerName));
	BroadcastCOMMODITYSTOCK();
        return true;
      
    ;
}

bool OutpostGameInfo::ENDDISCARDTOTURNEND_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::DoDiscard
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ENDDISCARDTOTURNEND";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::ENDDISCARDTOTURNEND_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::DoDiscard
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ENDDISCARDTOTURNEND";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().AllPlayersDone() && !GetOptions().GetEarlyDiscard()
      
    ;
}

bool OutpostGameInfo::ENDDISCARDTOTURNEND_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::DoDiscard
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ENDDISCARDTOTURNEND";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to ENDDISCARDTOTURNEND");
    return false;
  }

  return true;;
}

bool OutpostGameInfo::COMMODITYDONETOPURCHASE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::PromptMegaResources
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "COMMODITYDONETOPURCHASE";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::COMMODITYDONETOPURCHASE_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::PromptMegaResources
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "COMMODITYDONETOPURCHASE";
    throw std::out_of_range(oss.str());
  }

  return 
    
      GetProductionManager().IsProductionDone() && !GetOptions().GetEarlyDiscard()
    
    ;
}

bool OutpostGameInfo::COMMODITYDONETOPURCHASE_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::PromptMegaResources
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "COMMODITYDONETOPURCHASE";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to COMMODITYDONETOPURCHASE");
    return false;
  }

  return true;;
}

bool OutpostGameInfo::UNJOIN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "UNJOIN";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().IsPlayer(i_PlayerName)
      
    ;
}

bool OutpostGameInfo::UNJOIN_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "UNJOIN";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::UNJOIN_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "UNJOIN";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to UNJOIN");
    return false;
  }

  
      GetPlayers().remove(i_PlayerName);

      BroadcastRESET();
      BroadcastPLAYER();
      AllCastIAM();

      return true;
    ;
}

bool OutpostGameInfo::DONE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "DONE";
    throw std::out_of_range(oss.str());
  }

  return  
      
        GetPlayers().IsPlayer(i_PlayerName) &&
	(GetPlayers().GetPlayerByName(i_PlayerName).GetPurchaseState() == INTERNALIZING ||
	 GetPlayers().GetPlayerByName(i_PlayerName).GetPurchaseState() == ACTIVE) &&
	 GetPlayers().GetPlayerByName(i_PlayerName).GetBidState() != CURBID &&
	 GetPlayers().GetPlayerByName(i_PlayerName).GetHighBid() == NOT_HIGH_BID
      
    ;
}

bool OutpostGameInfo::DONE_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "DONE";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::DONE_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "DONE";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to DONE");
    return false;
  }

    
      
        size_t nextplayerid = NO_PLAYER;
        if (GetPlayers().GetPlayerByName(i_PlayerName).GetPurchaseState() == ACTIVE)
	{
	  GetPlayers().PurchaseIncrementTurnOrder();
	  if (!GetPlayers().AllPlayersDone()) nextplayerid = GetPlayers().GetCurTurnPlayer().GetId();
	}
	else
	{
	  GetPlayers().GetPlayerByName(i_PlayerName).SetPurchaseState(INTERNAL_DONE);
	}

	GetPlayers().GetPlayerByName(i_PlayerName).SetBidState(PASS_OUT);

	BroadcastUpdatePLAYERSTATEByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	if (nextplayerid != NO_PLAYER) BroadcastUpdatePLAYERSTATEByplayerindex( nextplayerid );
	return true;
      
    ;
}

bool OutpostGameInfo::NEWTURN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::TurnEnd
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "NEWTURN";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::NEWTURN_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::TurnEnd
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "NEWTURN";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().GetWinningPlayerIndices().size() == 0
      
    ;
}

bool OutpostGameInfo::NEWTURN_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::TurnEnd
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "NEWTURN";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to NEWTURN");
    return false;
  }

  
      
        SetCurTurn(GetCurTurn() + 1);

	size_t i;
	for (i = 0 ; i < GetPlayers().size() ; ++i)
	{
	  GetPlayers()[i].GetCommodityHand().AgeHand();
	  GetPlayers()[i].SetLastBid(NOT_HIGH_BID);
	  GetPlayers()[i].SetHasPurchased(false);
	}

	BroadcastCURTURN();
	return true;
      
    ;
}

bool OutpostGameInfo::STARTGAME_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::ValidateSetup
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STARTGAME";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::STARTGAME_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::ValidateSetup
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STARTGAME";
    throw std::out_of_range(oss.str());
  }

  return 
      GetPlayers().AllOk()
    ;
}

bool OutpostGameInfo::STARTGAME_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::ValidateSetup
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STARTGAME";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to STARTGAME");
    return false;
  }

  
      
        SetCurTurn(1);
	GetCommodities().ApplyResearchMegaStatus(GetOptions().GetResearchMega());
	GetUnownedItems().Initialize(GetPlayers().size(),GetOptions().GetStockMechanism());
	size_t i;
	for (i = 0 ; i < GetPlayers().size() ; ++i)
	{
	  GetPlayers()[i].GetFactories().AddFactory(ORE_FACTORY);
	  GetPlayers()[i].GetFactories().AddFactory(ORE_FACTORY);
	  GetPlayers()[i].GetFactories().AddFactory(WATER_FACTORY);
	  GetPlayers()[i].GetFactories().AlterManning("HHH",3,0);
	  GetPlayers()[i].AddHumans(STARTHUMANS);
	}

	BroadcastCURTURN();
	BroadcastITEMSTOCK();
	BroadcastPLAYERFACTORIES();
	BroadcastPLAYERSTATE();
	BroadcastCOMMODITYSTOCK();
	return true;
      
    ;
}

bool OutpostGameInfo::STANDARDBASIC_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STANDARDBASIC";
    throw std::out_of_range(oss.str());
  }

  return 
      GetPlayers().IsPlayer(i_PlayerName)
    ;
}

bool OutpostGameInfo::STANDARDBASIC_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STANDARDBASIC";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::STANDARDBASIC_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STANDARDBASIC";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to STANDARDBASIC");
    return false;
  }

  
      GetOptions().SetStandardBasic();
      BroadcastOPTIONS();
      return true;		   
    ;
}

bool OutpostGameInfo::BID_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BID";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().GetCurBidPlayer().GetName() == i_PlayerName &&
	GetPlayers().GetCurBidPlayer().GetHighBid() == NOT_HIGH_BID &&
	GetPlayers().GetCurBidPlayer().GetCommodityHand().GetHandValue() > 
	    GetPlayers().GetCurrentHighBid() -
	    GetPlayers().GetCurBidPlayer().GetOwnedItems().GetDiscount(
	        GetUnownedItems().GetShipItem(GetUnownedItems().GetBidIndex()))
      
    ;
}

bool OutpostGameInfo::BID_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BID";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::BID_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BID";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 1)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to BID");
    return false;
  }

  int i_bid(boost::lexical_cast<int>(i_ap[0]));

  if (i_bid < GetPlayers().GetCurrentHighBid() + 1)
  {
    UnicastERROR(i_PlayerName,"Action BID variable i_bid is below minimum");
    return false;
  }

  if (i_bid > GetPlayers().GetCurBidPlayer().GetCommodityHand().GetHandValue() +          GetPlayers().GetCurBidPlayer().GetOwnedItems().GetDiscount(            GetUnownedItems().GetShipItem(GetUnownedItems().GetBidIndex())))
  {
    UnicastERROR(i_PlayerName,"Action BID variable i_bid is above maximum");
    return false;
  }

  
      
        // find who had the high bid before.

	size_t highindex = GetPlayers().GetHighBidPlayer().GetId();
	size_t oldindex = GetPlayers().GetCurBidPlayer().GetId();

        GetPlayers().IncrementBid(HASBID,i_bid);

	GetPlayers().GetPlayerByName(i_PlayerName).SetLastBid(i_bid);

	BroadcastUpdatePLAYERSTATEByplayerindex(highindex);
	BroadcastUpdatePLAYERSTATEByplayerindex(oldindex);
	BroadcastUpdatePLAYERSTATEByplayerindex(GetPlayers().GetCurBidPlayer().GetId());
	BroadcastPURCHASEPRICE();
        return true;
      
    ;
}

bool OutpostGameInfo::STARTPURCHASE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::StartPurchase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STARTPURCHASE";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::STARTPURCHASE_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::StartPurchase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STARTPURCHASE";
    throw std::out_of_range(oss.str());
  }

  return true;
}

bool OutpostGameInfo::STARTPURCHASE_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::StartPurchase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STARTPURCHASE";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to STARTPURCHASE");
    return false;
  }

  
      
        GetPlayers().StartPurchasePhase();
	BroadcastPLAYERSTATE();
	AllCastPLAYERPURCHASEINFO();
	return true;
      
    ;
}

bool OutpostGameInfo::COMMODITYDONETODISCARD_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::PromptMegaResources
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "COMMODITYDONETODISCARD";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::COMMODITYDONETODISCARD_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::PromptMegaResources
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "COMMODITYDONETODISCARD";
    throw std::out_of_range(oss.str());
  }

  return 
    
      GetProductionManager().IsProductionDone() && GetOptions().GetEarlyDiscard()
    
    ;
}

bool OutpostGameInfo::COMMODITYDONETODISCARD_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::PromptMegaResources
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "COMMODITYDONETODISCARD";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to COMMODITYDONETODISCARD");
    return false;
  }

  return true;;
}

bool OutpostGameInfo::NUMMEGAS_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::MegaResources
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "NUMMEGAS";
    throw std::out_of_range(oss.str());
  }

  return  
      i_PlayerName == GetPlayers().GetCurTurnPlayer().GetName()
    ;
}

bool OutpostGameInfo::NUMMEGAS_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::MegaResources
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "NUMMEGAS";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::NUMMEGAS_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::MegaResources
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "NUMMEGAS";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 1)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to NUMMEGAS");
    return false;
  }

  int i_count(boost::lexical_cast<int>(i_ap[0]));

  if (i_count < 0)
  {
    UnicastERROR(i_PlayerName,"Action NUMMEGAS variable i_count is below minimum");
    return false;
  }

  if (i_count > GetProductionManager().GetMegaInfo().m_MaxMegas)
  {
    UnicastERROR(i_PlayerName,"Action NUMMEGAS variable i_count is above maximum");
    return false;
  }

  
      
      GetProductionManager().ContinueProduction(i_count,
                                             GetPlayers(),
                                             GetCommodities(),
					     GetCurTurn() == 1,
					     GetOptions().GetRefineries());

      BroadcastPLAYERCARDBACKS();
      AllCastPLAYERCARDS();
      BroadcastPLAYERSTATE();
      BroadcastCOMMODITYSTOCK();      

      return true;
      
    ;
}

bool OutpostGameInfo::STARTDISCARD_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::StartDiscard
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STARTDISCARD";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::STARTDISCARD_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::StartDiscard
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STARTDISCARD";
    throw std::out_of_range(oss.str());
  }

  return true;
}

bool OutpostGameInfo::STARTDISCARD_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::StartDiscard
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STARTDISCARD";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to STARTDISCARD");
    return false;
  }

  
      GetPlayers().StartTurnOrder();
      BroadcastPLAYERSTATE();

      return true;
    ;
}

bool OutpostGameInfo::MANUALDISCARD_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::DoDiscard
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "MANUALDISCARD";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::MANUALDISCARD_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::DoDiscard
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "MANUALDISCARD";
    throw std::out_of_range(oss.str());
  }

  return 
      
        !GetPlayers().AllPlayersDone() &&
         GetPlayers().GetCurTurnPlayer().GetHandLimit(GetOptions().GetEarlyDiscard()) <
         GetPlayers().GetCurTurnPlayer().GetCommodityHand().GetHandWeight(GetOptions().GetResearchIsSmall(),
      						                          GetOptions().GetMicroIsSmall())
      
    ;
}

bool OutpostGameInfo::MANUALDISCARD_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::DoDiscard
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "MANUALDISCARD";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to MANUALDISCARD");
    return false;
  }

  
      
        return true;
      
    ;
}

bool OutpostGameInfo::PASSOUT_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "PASSOUT";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().IsPlayer(i_PlayerName) && 
	GetPlayers().GetPlayerByName(i_PlayerName).GetHighBid() == NOT_HIGH_BID &&
	GetPlayers().GetPlayerByName(i_PlayerName).GetBidState() != PASS_OUT
      
    ;
}

bool OutpostGameInfo::PASSOUT_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "PASSOUT";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::PASSOUT_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "PASSOUT";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to PASSOUT");
    return false;
  }

  
      
      if (i_PlayerName == GetPlayers().GetCurBidPlayer().GetName())
      {     
        size_t oldindex = GetPlayers().GetCurBidPlayer().GetId();
        GetPlayers().IncrementBid(PASS_OUT);

	BroadcastUpdatePLAYERSTATEByplayerindex(oldindex);
	BroadcastUpdatePLAYERSTATEByplayerindex(GetPlayers().GetCurBidPlayer().GetId());
      }
      else
      {
        GetPlayers().GetPlayerByName(i_PlayerName).SetBidState(PASS_OUT);
	BroadcastUpdatePLAYERSTATEByplayerindex(GetPlayers().GetPlayerByName(i_PlayerName).GetId());
      }

      return true;
      
    ;
}

bool OutpostGameInfo::ENDMANNINGTOTURNEND_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::FinalManning
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ENDMANNINGTOTURNEND";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::ENDMANNINGTOTURNEND_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::FinalManning
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ENDMANNINGTOTURNEND";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().AllPlayersDone() && GetOptions().GetEarlyDiscard()
      
    ;
}

bool OutpostGameInfo::ENDMANNINGTOTURNEND_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::FinalManning
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ENDMANNINGTOTURNEND";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to ENDMANNINGTOTURNEND");
    return false;
  }

  return true;;
}

bool OutpostGameInfo::LIKEOPTIONS_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::ValidateSetup
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "LIKEOPTIONS";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().IsPlayer(i_PlayerName) && 
        !GetPlayers().GetPlayerByName(i_PlayerName).OkStartup()
      
    ;
}

bool OutpostGameInfo::LIKEOPTIONS_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::ValidateSetup
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "LIKEOPTIONS";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::LIKEOPTIONS_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::ValidateSetup
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "LIKEOPTIONS";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to LIKEOPTIONS");
    return false;
  }

  
      GetPlayers().GetPlayerByName(i_PlayerName).SetOkStartup(true);
      BroadcastUpdatePLAYEROKSTARTUPByplayerindex(GetPlayers().GetPlayerByName(i_PlayerName).GetId());
      return true;
    ;
}

bool OutpostGameInfo::OPTIONS_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "OPTIONS";
    throw std::out_of_range(oss.str());
  }

  return 
      GetPlayers().IsPlayer(i_PlayerName)
    ;
}

bool OutpostGameInfo::OPTIONS_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "OPTIONS";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::OPTIONS_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "OPTIONS";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 11)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to OPTIONS");
    return false;
  }

  bool discard(boost::lexical_cast<bool>(i_ap[0]));

  bool refinery(boost::lexical_cast<bool>(i_ap[1]));

  bool oneupgrade(boost::lexical_cast<bool>(i_ap[2]));

  bool activebid(boost::lexical_cast<bool>(i_ap[3]));

  int supplyharshness(boost::lexical_cast<int>(i_ap[4]));

  if (supplyharshness < 0)
  {
    UnicastERROR(i_PlayerName,"Action OPTIONS variable supplyharshness is below minimum");
    return false;
  }

  int robots(boost::lexical_cast<int>(i_ap[5]));

  if (robots < 1)
  {
    UnicastERROR(i_PlayerName,"Action OPTIONS variable robots is below minimum");
    return false;
  }

  if (robots > 3)
  {
    UnicastERROR(i_PlayerName,"Action OPTIONS variable robots is above maximum");
    return false;
  }

  bool researchmega(boost::lexical_cast<bool>(i_ap[6]));

  bool smallresearch(boost::lexical_cast<bool>(i_ap[7]));

  bool smallmicro(boost::lexical_cast<bool>(i_ap[8]));

  bool blinddraws(boost::lexical_cast<bool>(i_ap[9]));

  int stock(boost::lexical_cast<int>(i_ap[10]));

  if (stock < 1)
  {
    UnicastERROR(i_PlayerName,"Action OPTIONS variable stock is below minimum");
    return false;
  }

  if (stock > 2)
  {
    UnicastERROR(i_PlayerName,"Action OPTIONS variable stock is above maximum");
    return false;
  }

  
      GetOptions().SetEarlyDiscard(discard);
      GetOptions().SetRefineries(refinery);
      GetOptions().SetOneUpgradePerTurn(oneupgrade);
      GetOptions().SetMustBeActiveToBid(activebid);
      GetOptions().SetSupplyHarshness(supplyharshness);
      GetOptions().SetRobotMechanism((RobotMechanism)robots);
      GetOptions().SetResearchMega(researchmega);
      GetOptions().SetResearchIsSmall(smallresearch);
      GetOptions().SetMicroIsSmall(smallmicro);
      GetOptions().SetBlindDraws(blinddraws);
      GetOptions().SetStockMechanism((StockMechanism)stock);

      BroadcastOPTIONS();
      return true;		   
    ;
}

bool OutpostGameInfo::ENDMANNINGTODISCARD_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::FinalManning
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ENDMANNINGTODISCARD";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::ENDMANNINGTODISCARD_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::FinalManning
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ENDMANNINGTODISCARD";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().AllPlayersDone() && !GetOptions().GetEarlyDiscard()
      
    ;
}

bool OutpostGameInfo::ENDMANNINGTODISCARD_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::FinalManning
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ENDMANNINGTODISCARD";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to ENDMANNINGTODISCARD");
    return false;
  }

  return true;;
}

bool OutpostGameInfo::BUYNEWCHEMFACTORIES_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYNEWCHEMFACTORIES";
    throw std::out_of_range(oss.str());
  }

  return  
      
        GetPlayers().CanPlayerPurchase(i_PlayerName,NEW_CHEMICALS_FACTORY_PURCHASABLE,GetOptions().GetRobotMechanism())
      
    ;
}

bool OutpostGameInfo::BUYNEWCHEMFACTORIES_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYNEWCHEMFACTORIES";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::BUYNEWCHEMFACTORIES_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYNEWCHEMFACTORIES";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 2)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to BUYNEWCHEMFACTORIES");
    return false;
  }

  int i_numfactories(boost::lexical_cast<int>(i_ap[0]));

  if (i_numfactories < 0)
  {
    UnicastERROR(i_PlayerName,"Action BUYNEWCHEMFACTORIES variable i_numfactories is below minimum");
    return false;
  }

  if (i_numfactories > GetPlayers().GetPlayerByName(i_PlayerName).GetMaxPurchasable(NEW_CHEMICALS_FACTORY_PURCHASABLE,GetOptions().GetRobotMechanism()))
  {
    UnicastERROR(i_PlayerName,"Action BUYNEWCHEMFACTORIES variable i_numfactories is above maximum");
    return false;
  }

  std::string i_discardstring(boost::lexical_cast<std::string>(i_ap[1]));

  
      
        Spend spend(GetCurTurn(),
	            GetPlayers().GetPlayerByName(i_PlayerName).GetId(),
		    ITEMPURCHASE,
		    NEW_CHEMICALS_FACTORY_PURCHASABLE
		    );

        HandDeleter hd(*this,GetPlayers().GetPlayerByName(i_PlayerName),
	               i_discardstring,spend,false,i_numfactories * 60,i_numfactories);
	if (!hd.IsOk()) return false;

	GetSpends().push_back(spend);
        BroadcastUpdateSPENDSByspendIndex(GetSpends().size()-1);

	for (int i = 0 ; i < i_numfactories ; ++i)
	{
	  GetPlayers().GetPlayerByName(i_PlayerName).GetFactories().AddFactory(NEW_CHEMICALS_FACTORY);
	}

	GetPlayers().GetPlayerByName(i_PlayerName).AutoMan();

	BroadcastUpdatePLAYERSTATEByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	BroadcastUpdatePLAYERFACTORIESByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	BroadcastUpdatePLAYERCARDBACKSByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	SingleCastPLAYERCARDS(GetPlayers().GetPlayerId(i_PlayerName) );
	SingleCastPLAYERPURCHASEINFO(GetPlayers().GetPlayerId(i_PlayerName));
	BroadcastCOMMODITYSTOCK();
        return true;
      
    ;
}

bool OutpostGameInfo::STARTMANNING_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::StartFinalManning
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STARTMANNING";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::STARTMANNING_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::StartFinalManning
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STARTMANNING";
    throw std::out_of_range(oss.str());
  }

  return true;
}

bool OutpostGameInfo::STARTMANNING_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::StartFinalManning
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STARTMANNING";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to STARTMANNING");
    return false;
  }

  
      
        GetPlayers().StartManningTurnOrder();
	BroadcastPLAYERSTATE();
	return true;
      
    ;
}

bool OutpostGameInfo::BUYROBOTS_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYROBOTS";
    throw std::out_of_range(oss.str());
  }

  return  
      
        GetPlayers().CanPlayerPurchase(i_PlayerName,ROBOT_PURCHASABLE,GetOptions().GetRobotMechanism())
      
    ;
}

bool OutpostGameInfo::BUYROBOTS_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYROBOTS";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::BUYROBOTS_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYROBOTS";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 2)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to BUYROBOTS");
    return false;
  }

  int i_numrobots(boost::lexical_cast<int>(i_ap[0]));

  if (i_numrobots < 0)
  {
    UnicastERROR(i_PlayerName,"Action BUYROBOTS variable i_numrobots is below minimum");
    return false;
  }

  if (i_numrobots > GetPlayers().GetPlayerByName(i_PlayerName).GetMaxPurchasable(ROBOT_PURCHASABLE,GetOptions().GetRobotMechanism()))
  {
    UnicastERROR(i_PlayerName,"Action BUYROBOTS variable i_numrobots is above maximum");
    return false;
  }

  std::string i_discardstring(boost::lexical_cast<std::string>(i_ap[1]));

  
      
        Spend spend(GetCurTurn(),
	            GetPlayers().GetPlayerByName(i_PlayerName).GetId(),
		    ITEMPURCHASE,
		    ROBOT_PURCHASABLE
		    );

        HandDeleter hd(*this,GetPlayers().GetPlayerByName(i_PlayerName),i_discardstring,spend,false,i_numrobots * 10);
	if (!hd.IsOk()) return false;

	GetSpends().push_back(spend);
        BroadcastUpdateSPENDSByspendIndex(GetSpends().size()-1);

	GetPlayers().GetPlayerByName(i_PlayerName).AddRobots(i_numrobots);
	GetPlayers().GetPlayerByName(i_PlayerName).AutoMan();

	BroadcastUpdatePLAYERSTATEByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	BroadcastUpdatePLAYERFACTORIESByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	BroadcastUpdatePLAYERCARDBACKSByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	SingleCastPLAYERCARDS(GetPlayers().GetPlayerId(i_PlayerName) );
	SingleCastPLAYERPURCHASEINFO(GetPlayers().GetPlayerId(i_PlayerName));
	BroadcastCOMMODITYSTOCK();
        return true;
      
    ;
}

bool OutpostGameInfo::INTERNALIZE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "INTERNALIZE";
    throw std::out_of_range(oss.str());
  }

  return  
      
        GetPlayers().IsPlayer(i_PlayerName) &&
	(GetPlayers().GetPlayerByName(i_PlayerName).GetPurchaseState() == UNPLAYED ||
	 GetPlayers().GetPlayerByName(i_PlayerName).GetPurchaseState() == ACTIVE) &&
	 GetPlayers().GetPlayerByName(i_PlayerName).GetBidState() != CURBID &&
	 GetPlayers().GetPlayerByName(i_PlayerName).GetHighBid() == NOT_HIGH_BID
      
    ;
}

bool OutpostGameInfo::INTERNALIZE_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "INTERNALIZE";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::INTERNALIZE_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "INTERNALIZE";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to INTERNALIZE");
    return false;
  }

  
      
        size_t curplayerid = GetPlayers().GetPlayerId(i_PlayerName);
	size_t nextplayerid = NO_PLAYER;

        if (GetPlayers().GetPlayerByName(i_PlayerName).GetPurchaseState() == ACTIVE)
	{
	  GetPlayers().PurchaseIncrementTurnOrder();
	  if (!GetPlayers().AllPlayersDone()) nextplayerid = GetPlayers().GetCurTurnPlayer().GetId();
	}
	GetPlayers().GetPlayerByName(i_PlayerName).SetPurchaseState(INTERNALIZING);
	GetPlayers().GetPlayerByName(i_PlayerName).SetBidState(PASS_OUT);

	BroadcastUpdatePLAYERSTATEByplayerindex( curplayerid );
	if (nextplayerid != NO_PLAYER)
	{
	  BroadcastUpdatePLAYERSTATEByplayerindex( nextplayerid );
	}

        return true;
      
    ;
}

bool OutpostGameInfo::ALTERMANNING_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
     && i_StateName != OutpostGameInfo::FinalManning
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ALTERMANNING";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().IsPlayer(i_PlayerName) &&
	(i_StateName != FinalManning || 
	  ( !GetPlayers().AllPlayersDone() && 
	     GetPlayers().GetCurTurnPlayer().GetName() == i_PlayerName
	  )
        )
      
    ;
}

bool OutpostGameInfo::ALTERMANNING_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
     && i_StateName != OutpostGameInfo::FinalManning
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ALTERMANNING";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::ALTERMANNING_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
     && i_StateName != OutpostGameInfo::FinalManning
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ALTERMANNING";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 1)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to ALTERMANNING");
    return false;
  }

  std::string i_manningstring(boost::lexical_cast<std::string>(i_ap[0]));

  
      
        Player &pl = GetPlayers().GetPlayerByName(i_PlayerName);
	int result = pl.GetFactories().AlterManning(i_manningstring,pl.GetNumHumans(),pl.GetNumRobots());
	switch(result)
	{
	case -4: UnicastERROR(i_PlayerName,"Removing Man from Mannable Item"); return false;
	case -3: UnicastERROR(i_PlayerName,"Invalid Character in Manning List"); return false;
	case -2: UnicastERROR(i_PlayerName,"Too Many Robots or People"); return false;
	case -1: UnicastERROR(i_PlayerName,"Invalid Length of Manning List"); return false;
	}
	BroadcastUpdatePLAYERFACTORIESByplayerindex(pl.GetId());
	BroadcastUpdatePLAYERSTATEByplayerindex(pl.GetId());
	SingleCastPLAYERPURCHASEINFO( pl.GetId() );
        return true;
      
    ;
}

bool OutpostGameInfo::ENDDISCARDTOPURCHASE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::DoDiscard
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ENDDISCARDTOPURCHASE";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::ENDDISCARDTOPURCHASE_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::DoDiscard
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ENDDISCARDTOPURCHASE";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().AllPlayersDone() && GetOptions().GetEarlyDiscard()
      
    ;
}

bool OutpostGameInfo::ENDDISCARDTOPURCHASE_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::DoDiscard
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ENDDISCARDTOPURCHASE";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to ENDDISCARDTOPURCHASE");
    return false;
  }

  return true;;
}

bool OutpostGameInfo::BUYRESEARCHFACTORIES_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYRESEARCHFACTORIES";
    throw std::out_of_range(oss.str());
  }

  return  
      
        GetPlayers().CanPlayerPurchase(i_PlayerName,RESEARCH_FACTORY_PURCHASABLE,GetOptions().GetRobotMechanism())
      
    ;
}

bool OutpostGameInfo::BUYRESEARCHFACTORIES_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYRESEARCHFACTORIES";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::BUYRESEARCHFACTORIES_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYRESEARCHFACTORIES";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 2)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to BUYRESEARCHFACTORIES");
    return false;
  }

  int i_numfactories(boost::lexical_cast<int>(i_ap[0]));

  if (i_numfactories < 0)
  {
    UnicastERROR(i_PlayerName,"Action BUYRESEARCHFACTORIES variable i_numfactories is below minimum");
    return false;
  }

  if (i_numfactories > GetPlayers().GetPlayerByName(i_PlayerName).GetMaxPurchasable(RESEARCH_FACTORY_PURCHASABLE,GetOptions().GetRobotMechanism()))
  {
    UnicastERROR(i_PlayerName,"Action BUYRESEARCHFACTORIES variable i_numfactories is above maximum");
    return false;
  }

  std::string i_discardstring(boost::lexical_cast<std::string>(i_ap[1]));

  
      
        Spend spend(GetCurTurn(),
	            GetPlayers().GetPlayerByName(i_PlayerName).GetId(),
		    ITEMPURCHASE,
		    RESEARCH_FACTORY_PURCHASABLE
		    );

        HandDeleter hd(*this,GetPlayers().GetPlayerByName(i_PlayerName),i_discardstring,spend,false,i_numfactories * 40);
	if (!hd.IsOk()) return false;

	GetSpends().push_back(spend);
        BroadcastUpdateSPENDSByspendIndex(GetSpends().size()-1);

	for (int i = 0 ; i < i_numfactories ; ++i)
	{
	  GetPlayers().GetPlayerByName(i_PlayerName).GetFactories().AddFactory(RESEARCH_FACTORY);
	}

	GetPlayers().GetPlayerByName(i_PlayerName).AutoMan();

	BroadcastUpdatePLAYERSTATEByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	BroadcastUpdatePLAYERFACTORIESByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	BroadcastUpdatePLAYERCARDBACKSByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	SingleCastPLAYERCARDS(GetPlayers().GetPlayerId(i_PlayerName) );
	SingleCastPLAYERPURCHASEINFO(GetPlayers().GetPlayerId(i_PlayerName));
	BroadcastCOMMODITYSTOCK();
        return true;
      
    ;
}

bool OutpostGameInfo::FINALDONE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "FINALDONE";
    throw std::out_of_range(oss.str());
  }

  return  
      
        GetPlayers().IsPlayer(i_PlayerName) &&
	(GetPlayers().GetPlayerByName(i_PlayerName).GetPurchaseState() == INTERNALIZING ||
	 GetPlayers().GetPlayerByName(i_PlayerName).GetPurchaseState() == ACTIVE) &&
	 GetPlayers().GetPlayerByName(i_PlayerName).GetBidState() != CURBID &&
	 GetPlayers().GetPlayerByName(i_PlayerName).GetHighBid() == NOT_HIGH_BID
      
    ;
}

bool OutpostGameInfo::FINALDONE_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "FINALDONE";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::FINALDONE_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "FINALDONE";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to FINALDONE");
    return false;
  }

    
      
        size_t nextplayerid = NO_PLAYER;
        if (GetPlayers().GetPlayerByName(i_PlayerName).GetPurchaseState() == ACTIVE)
	{
	  GetPlayers().PurchaseIncrementTurnOrder(FINAL_DONE);
	  if (!GetPlayers().AllPlayersDone()) nextplayerid = GetPlayers().GetCurTurnPlayer().GetId();
	}
	else
	{
	  GetPlayers().GetPlayerByName(i_PlayerName).SetPurchaseState(INTERNAL_FINAL_DONE);
	}

	GetPlayers().GetPlayerByName(i_PlayerName).SetBidState(PASS_OUT);

	BroadcastUpdatePLAYERSTATEByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	if (nextplayerid != NO_PLAYER) BroadcastUpdatePLAYERSTATEByplayerindex( nextplayerid );
	return true;
      
    ;
}

bool OutpostGameInfo::WATERMULLIGAN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "WATERMULLIGAN";
    throw std::out_of_range(oss.str());
  }

  return  
      
        GetPlayers().CanPlayerMulligan(i_PlayerName) && GetCurTurn() == 1
      
    ;
}

bool OutpostGameInfo::WATERMULLIGAN_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "WATERMULLIGAN";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::WATERMULLIGAN_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "WATERMULLIGAN";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to WATERMULLIGAN");
    return false;
  }

  
      
        Spend spend(GetCurTurn(),
	            GetPlayers().GetPlayerByName(i_PlayerName).GetId(),
		    WATERMULLIGAN,
		    WATER_FACTORY_PURCHASABLE
		    );
	spend.SetSpendCost(0);

        size_t i;
	for (i = 0 ; i < GetPlayers().GetPlayerByName(i_PlayerName).GetCommodityHand().GetHand().size() ; ++i)
	{
	  Commodity c = GetPlayers().GetPlayerByName(i_PlayerName).GetCommodityHand().GetHand()[i];
	  GetCommodities().GetDeck(c.GetType()).DiscardCommodity(c);
	  spend.AddCommodity(c);	  
	}
	GetPlayers().GetPlayerByName(i_PlayerName).GetCommodityHand() = CommodityHand();
	GetPlayers().GetPlayerByName(i_PlayerName).GetFactories().AddFactory(WATER_FACTORY);

	GetSpends().push_back(spend);
        BroadcastUpdateSPENDSByspendIndex(GetSpends().size()-1);

	GetPlayers().GetPlayerByName(i_PlayerName).AutoMan();

	BroadcastUpdatePLAYERSTATEByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	BroadcastUpdatePLAYERFACTORIESByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	BroadcastUpdatePLAYERCARDBACKSByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	SingleCastPLAYERCARDS(GetPlayers().GetPlayerId(i_PlayerName) );
	SingleCastPLAYERPURCHASEINFO(GetPlayers().GetPlayerId(i_PlayerName));
	BroadcastCOMMODITYSTOCK();
        return true;
      
    ;
}

bool OutpostGameInfo::INITCOMMODITY_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::StartResources
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "INITCOMMODITY";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::INITCOMMODITY_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::StartResources
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "INITCOMMODITY";
    throw std::out_of_range(oss.str());
  }

  return true;
}

bool OutpostGameInfo::INITCOMMODITY_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::StartResources
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "INITCOMMODITY";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to INITCOMMODITY");
    return false;
  }

  
      
      GetProductionManager().StartProduction(GetPlayers(),
                                             GetCommodities(),
					     GetCurTurn() == 1,
					     GetOptions().GetRefineries());

      BroadcastPLAYERCARDBACKS();
      AllCastPLAYERCARDS();
      BroadcastPLAYERSTATE();
      BroadcastCOMMODITYSTOCK();

      return true;
      
    ;
}

bool OutpostGameInfo::STANDARDEXPERT_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STANDARDEXPERT";
    throw std::out_of_range(oss.str());
  }

  return 
      GetPlayers().IsPlayer(i_PlayerName)
    ;
}

bool OutpostGameInfo::STANDARDEXPERT_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STANDARDEXPERT";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::STANDARDEXPERT_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STANDARDEXPERT";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to STANDARDEXPERT");
    return false;
  }

  
      GetOptions().SetStandardExpert();
      BroadcastOPTIONS();
      return true;		   
    ;
}

bool OutpostGameInfo::BUYMEN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYMEN";
    throw std::out_of_range(oss.str());
  }

  return  
      
        GetPlayers().CanPlayerPurchase(i_PlayerName,MAN_PURCHASABLE,GetOptions().GetRobotMechanism())
      
    ;
}

bool OutpostGameInfo::BUYMEN_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYMEN";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::BUYMEN_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::Purchase
     && i_StateName != OutpostGameInfo::Bid
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUYMEN";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 2)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to BUYMEN");
    return false;
  }

  int i_nummen(boost::lexical_cast<int>(i_ap[0]));

  if (i_nummen < 0)
  {
    UnicastERROR(i_PlayerName,"Action BUYMEN variable i_nummen is below minimum");
    return false;
  }

  if (i_nummen > GetPlayers().GetPlayerByName(i_PlayerName).GetMaxPurchasable(MAN_PURCHASABLE,GetOptions().GetRobotMechanism()))
  {
    UnicastERROR(i_PlayerName,"Action BUYMEN variable i_nummen is above maximum");
    return false;
  }

  std::string i_discardstring(boost::lexical_cast<std::string>(i_ap[1]));

  
      
        Spend spend(GetCurTurn(),
	            GetPlayers().GetPlayerByName(i_PlayerName).GetId(),
		    ITEMPURCHASE,
		    MAN_PURCHASABLE
		    );

        HandDeleter hd(*this,GetPlayers().GetPlayerByName(i_PlayerName),i_discardstring,spend,false,
	               i_nummen * GetPlayers().GetPlayerByName(i_PlayerName).GetOwnedItems().GetPeopleCost());
	if (!hd.IsOk()) return false;

	GetSpends().push_back(spend);
        BroadcastUpdateSPENDSByspendIndex(GetSpends().size()-1);
	GetPlayers().GetPlayerByName(i_PlayerName).AddHumans(i_nummen);
	GetPlayers().GetPlayerByName(i_PlayerName).AutoMan();

	BroadcastUpdatePLAYERSTATEByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	BroadcastUpdatePLAYERFACTORIESByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	BroadcastUpdatePLAYERCARDBACKSByplayerindex( GetPlayers().GetPlayerByName(i_PlayerName).GetId() );
	SingleCastPLAYERCARDS(GetPlayers().GetPlayerId(i_PlayerName) );
	SingleCastPLAYERPURCHASEINFO(GetPlayers().GetPlayerId(i_PlayerName));
	BroadcastCOMMODITYSTOCK();

	return true;
      
    ;
}

bool OutpostGameInfo::PURCHASEDONE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "PURCHASEDONE";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::PURCHASEDONE_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::Purchase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "PURCHASEDONE";
    throw std::out_of_range(oss.str());
  }

  return GetPlayers().PurchaseAllPlayersDone();
}

bool OutpostGameInfo::PURCHASEDONE_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::Purchase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "PURCHASEDONE";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to PURCHASEDONE");
    return false;
  }

  return true;;
}

bool OutpostGameInfo::GAMEOVER_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::TurnEnd
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GAMEOVER";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::GAMEOVER_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::TurnEnd
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GAMEOVER";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().GetWinningPlayerIndices().size() > 0
      
    ;
}

bool OutpostGameInfo::GAMEOVER_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::TurnEnd
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GAMEOVER";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to GAMEOVER");
    return false;
  }

  
      
        std::set<size_t> winners = GetPlayers().GetWinningPlayerIndices();
        std::set<size_t>::iterator winit;
	for (winit = winners.begin() ; winit != winners.end() ; ++winit)
	{
	  BroadcastPLAYERWINS(*winit);
	}
        return true;
      
    ;
}

bool OutpostGameInfo::REQUESTMEGA_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::PromptMegaResources
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "REQUESTMEGA";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool OutpostGameInfo::REQUESTMEGA_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != OutpostGameInfo::PromptMegaResources
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "REQUESTMEGA";
    throw std::out_of_range(oss.str());
  }

  return !GetProductionManager().IsProductionDone();
}

bool OutpostGameInfo::REQUESTMEGA_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != OutpostGameInfo::PromptMegaResources
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "REQUESTMEGA";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to REQUESTMEGA");
    return false;
  }

  
      
      UnicastMEGAREQUEST(GetPlayers().GetCurTurnPlayer().GetName());
      return true;
      
    ;
}


void OutpostGameInfo::SendFullState(const std::string &i_PlayerName,const StateName &i_StateName) const
{

    

      UnicastRESET(i_PlayerName);
      UnicastOPTIONS(i_PlayerName);
      UnicastPLAYER(i_PlayerName);

      if (GetPlayers().IsPlayer(i_PlayerName)) 
      {
        SingleCastIAM(GetPlayers().GetPlayerId(i_PlayerName));
      }


      if (i_StateName == ValidateSetup)
      {
        UnicastPLAYEROKSTARTUP(i_PlayerName);
      }


      if (i_StateName == InitialState || i_StateName == ValidateSetup) return;

      UnicastCURTURN(i_PlayerName);
      UnicastITEMSTOCK(i_PlayerName);
      UnicastTURNORDER(i_PlayerName);
      UnicastSUPPLYSHIP(i_PlayerName);      
      UnicastPLAYERSTATE(i_PlayerName);
      UnicastPLAYERFACTORIES(i_PlayerName);
      UnicastPLAYERCARDBACKS(i_PlayerName);
      UnicastPLAYERITEMS(i_PlayerName);
      UnicastCOMMODITYSTOCK(i_PlayerName);
      UnicastPURCHASEDATA(i_PlayerName);
      UnicastSPENDS(i_PlayerName);

      if (GetPlayers().IsPlayer(i_PlayerName)) 
      {
        SingleCastPLAYERCARDS(GetPlayers().GetPlayerId(i_PlayerName));
	SingleCastPLAYERPURCHASEINFO(GetPlayers().GetPlayerId(i_PlayerName));
      }

      if (i_StateName == MegaResources)
      {
	if (i_PlayerName == GetPlayers().GetCurTurnPlayer().GetName())
	{
          UnicastMEGAREQUEST(i_PlayerName);
        }
      }

      if (i_StateName == Bid)
      {
        UnicastBIDITEM(i_PlayerName);
	UnicastPURCHASEPRICE(i_PlayerName);
      }

      if (i_StateName == TerminalState)
      {
        std::set<size_t> winners = GetPlayers().GetWinningPlayerIndices();
        std::set<size_t>::iterator winit;
	for (winit = winners.begin() ; winit != winners.end() ; ++winit)
	{
	  UnicastPLAYERWINS(i_PlayerName,*winit);
	}
      }
    
  
}

const StateName OutpostGameInfo::Bid = "Bid";
const StateName OutpostGameInfo::FinalManning = "FinalManning";
const StateName OutpostGameInfo::MegaResources = "MegaResources";
const StateName OutpostGameInfo::ManualDiscard = "ManualDiscard";
const StateName OutpostGameInfo::TurnEnd = "TurnEnd";
const StateName OutpostGameInfo::TerminalState = "TerminalState";
const StateName OutpostGameInfo::TurnOrder = "TurnOrder";
const StateName OutpostGameInfo::InitialState = "InitialState";
const StateName OutpostGameInfo::StartResources = "StartResources";
const StateName OutpostGameInfo::DoDiscard = "DoDiscard";
const StateName OutpostGameInfo::SupplyFill = "SupplyFill";
const StateName OutpostGameInfo::StartPurchase = "StartPurchase";
const StateName OutpostGameInfo::ValidateSetup = "ValidateSetup";
const StateName OutpostGameInfo::PromptMegaResources = "PromptMegaResources";
const StateName OutpostGameInfo::Purchase = "Purchase";
const StateName OutpostGameInfo::StartDiscard = "StartDiscard";
const StateName OutpostGameInfo::StartFinalManning = "StartFinalManning";
typedef DLLGame<OutpostSet,OutpostGameInfo> OutpostGame;

bool PASS_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.PASS_IsLegal(i_PlayerName,i_StateName);
}

bool PASS_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.PASS_IsAuto(i_StateName);
}

bool PASS_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.PASS_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string PASS_Transition::GetName() const
{
  return "PASS";
}

bool FILLSHIP_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.FILLSHIP_IsLegal(i_PlayerName,i_StateName);
}

bool FILLSHIP_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.FILLSHIP_IsAuto(i_StateName);
}

bool FILLSHIP_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.FILLSHIP_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string FILLSHIP_Transition::GetName() const
{
  return "FILLSHIP";
}

bool JOIN_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.JOIN_IsLegal(i_PlayerName,i_StateName);
}

bool JOIN_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.JOIN_IsAuto(i_StateName);
}

bool JOIN_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.JOIN_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string JOIN_Transition::GetName() const
{
  return "JOIN";
}

bool OPENBID_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.OPENBID_IsLegal(i_PlayerName,i_StateName);
}

bool OPENBID_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.OPENBID_IsAuto(i_StateName);
}

bool OPENBID_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.OPENBID_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string OPENBID_Transition::GetName() const
{
  return "OPENBID";
}

bool BUYOREFACTORIES_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.BUYOREFACTORIES_IsLegal(i_PlayerName,i_StateName);
}

bool BUYOREFACTORIES_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.BUYOREFACTORIES_IsAuto(i_StateName);
}

bool BUYOREFACTORIES_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.BUYOREFACTORIES_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string BUYOREFACTORIES_Transition::GetName() const
{
  return "BUYOREFACTORIES";
}

bool DONTLIKEOPTIONS_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.DONTLIKEOPTIONS_IsLegal(i_PlayerName,i_StateName);
}

bool DONTLIKEOPTIONS_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.DONTLIKEOPTIONS_IsAuto(i_StateName);
}

bool DONTLIKEOPTIONS_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.DONTLIKEOPTIONS_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string DONTLIKEOPTIONS_Transition::GetName() const
{
  return "DONTLIKEOPTIONS";
}

bool DISCARD_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.DISCARD_IsLegal(i_PlayerName,i_StateName);
}

bool DISCARD_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.DISCARD_IsAuto(i_StateName);
}

bool DISCARD_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.DISCARD_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string DISCARD_Transition::GetName() const
{
  return "DISCARD";
}

bool PURCHASEBID_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.PURCHASEBID_IsLegal(i_PlayerName,i_StateName);
}

bool PURCHASEBID_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.PURCHASEBID_IsAuto(i_StateName);
}

bool PURCHASEBID_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.PURCHASEBID_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string PURCHASEBID_Transition::GetName() const
{
  return "PURCHASEBID";
}

bool AUTOSKIPDISCARD_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.AUTOSKIPDISCARD_IsLegal(i_PlayerName,i_StateName);
}

bool AUTOSKIPDISCARD_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.AUTOSKIPDISCARD_IsAuto(i_StateName);
}

bool AUTOSKIPDISCARD_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.AUTOSKIPDISCARD_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string AUTOSKIPDISCARD_Transition::GetName() const
{
  return "AUTOSKIPDISCARD";
}

bool BUYTITANIUMFACTORIES_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.BUYTITANIUMFACTORIES_IsLegal(i_PlayerName,i_StateName);
}

bool BUYTITANIUMFACTORIES_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.BUYTITANIUMFACTORIES_IsAuto(i_StateName);
}

bool BUYTITANIUMFACTORIES_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.BUYTITANIUMFACTORIES_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string BUYTITANIUMFACTORIES_Transition::GetName() const
{
  return "BUYTITANIUMFACTORIES";
}

bool DETERMINETURNORDER_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.DETERMINETURNORDER_IsLegal(i_PlayerName,i_StateName);
}

bool DETERMINETURNORDER_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.DETERMINETURNORDER_IsAuto(i_StateName);
}

bool DETERMINETURNORDER_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.DETERMINETURNORDER_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string DETERMINETURNORDER_Transition::GetName() const
{
  return "DETERMINETURNORDER";
}

bool DONEMANNING_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.DONEMANNING_IsLegal(i_PlayerName,i_StateName);
}

bool DONEMANNING_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.DONEMANNING_IsAuto(i_StateName);
}

bool DONEMANNING_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.DONEMANNING_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string DONEMANNING_Transition::GetName() const
{
  return "DONEMANNING";
}

bool VALIDATESETUP_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.VALIDATESETUP_IsLegal(i_PlayerName,i_StateName);
}

bool VALIDATESETUP_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.VALIDATESETUP_IsAuto(i_StateName);
}

bool VALIDATESETUP_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.VALIDATESETUP_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string VALIDATESETUP_Transition::GetName() const
{
  return "VALIDATESETUP";
}

bool BUYWATERFACTORIES_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.BUYWATERFACTORIES_IsLegal(i_PlayerName,i_StateName);
}

bool BUYWATERFACTORIES_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.BUYWATERFACTORIES_IsAuto(i_StateName);
}

bool BUYWATERFACTORIES_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.BUYWATERFACTORIES_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string BUYWATERFACTORIES_Transition::GetName() const
{
  return "BUYWATERFACTORIES";
}

bool ENDDISCARDTOTURNEND_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.ENDDISCARDTOTURNEND_IsLegal(i_PlayerName,i_StateName);
}

bool ENDDISCARDTOTURNEND_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.ENDDISCARDTOTURNEND_IsAuto(i_StateName);
}

bool ENDDISCARDTOTURNEND_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.ENDDISCARDTOTURNEND_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string ENDDISCARDTOTURNEND_Transition::GetName() const
{
  return "ENDDISCARDTOTURNEND";
}

bool COMMODITYDONETOPURCHASE_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.COMMODITYDONETOPURCHASE_IsLegal(i_PlayerName,i_StateName);
}

bool COMMODITYDONETOPURCHASE_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.COMMODITYDONETOPURCHASE_IsAuto(i_StateName);
}

bool COMMODITYDONETOPURCHASE_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.COMMODITYDONETOPURCHASE_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string COMMODITYDONETOPURCHASE_Transition::GetName() const
{
  return "COMMODITYDONETOPURCHASE";
}

bool UNJOIN_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.UNJOIN_IsLegal(i_PlayerName,i_StateName);
}

bool UNJOIN_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.UNJOIN_IsAuto(i_StateName);
}

bool UNJOIN_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.UNJOIN_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string UNJOIN_Transition::GetName() const
{
  return "UNJOIN";
}

bool DONE_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.DONE_IsLegal(i_PlayerName,i_StateName);
}

bool DONE_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.DONE_IsAuto(i_StateName);
}

bool DONE_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.DONE_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string DONE_Transition::GetName() const
{
  return "DONE";
}

bool NEWTURN_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.NEWTURN_IsLegal(i_PlayerName,i_StateName);
}

bool NEWTURN_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.NEWTURN_IsAuto(i_StateName);
}

bool NEWTURN_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.NEWTURN_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string NEWTURN_Transition::GetName() const
{
  return "NEWTURN";
}

bool STARTGAME_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.STARTGAME_IsLegal(i_PlayerName,i_StateName);
}

bool STARTGAME_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.STARTGAME_IsAuto(i_StateName);
}

bool STARTGAME_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.STARTGAME_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string STARTGAME_Transition::GetName() const
{
  return "STARTGAME";
}

bool STANDARDBASIC_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.STANDARDBASIC_IsLegal(i_PlayerName,i_StateName);
}

bool STANDARDBASIC_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.STANDARDBASIC_IsAuto(i_StateName);
}

bool STANDARDBASIC_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.STANDARDBASIC_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string STANDARDBASIC_Transition::GetName() const
{
  return "STANDARDBASIC";
}

bool BID_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.BID_IsLegal(i_PlayerName,i_StateName);
}

bool BID_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.BID_IsAuto(i_StateName);
}

bool BID_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.BID_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string BID_Transition::GetName() const
{
  return "BID";
}

bool STARTPURCHASE_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.STARTPURCHASE_IsLegal(i_PlayerName,i_StateName);
}

bool STARTPURCHASE_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.STARTPURCHASE_IsAuto(i_StateName);
}

bool STARTPURCHASE_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.STARTPURCHASE_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string STARTPURCHASE_Transition::GetName() const
{
  return "STARTPURCHASE";
}

bool COMMODITYDONETODISCARD_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.COMMODITYDONETODISCARD_IsLegal(i_PlayerName,i_StateName);
}

bool COMMODITYDONETODISCARD_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.COMMODITYDONETODISCARD_IsAuto(i_StateName);
}

bool COMMODITYDONETODISCARD_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.COMMODITYDONETODISCARD_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string COMMODITYDONETODISCARD_Transition::GetName() const
{
  return "COMMODITYDONETODISCARD";
}

bool NUMMEGAS_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.NUMMEGAS_IsLegal(i_PlayerName,i_StateName);
}

bool NUMMEGAS_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.NUMMEGAS_IsAuto(i_StateName);
}

bool NUMMEGAS_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.NUMMEGAS_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string NUMMEGAS_Transition::GetName() const
{
  return "NUMMEGAS";
}

bool STARTDISCARD_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.STARTDISCARD_IsLegal(i_PlayerName,i_StateName);
}

bool STARTDISCARD_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.STARTDISCARD_IsAuto(i_StateName);
}

bool STARTDISCARD_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.STARTDISCARD_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string STARTDISCARD_Transition::GetName() const
{
  return "STARTDISCARD";
}

bool MANUALDISCARD_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.MANUALDISCARD_IsLegal(i_PlayerName,i_StateName);
}

bool MANUALDISCARD_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.MANUALDISCARD_IsAuto(i_StateName);
}

bool MANUALDISCARD_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.MANUALDISCARD_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string MANUALDISCARD_Transition::GetName() const
{
  return "MANUALDISCARD";
}

bool PASSOUT_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.PASSOUT_IsLegal(i_PlayerName,i_StateName);
}

bool PASSOUT_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.PASSOUT_IsAuto(i_StateName);
}

bool PASSOUT_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.PASSOUT_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string PASSOUT_Transition::GetName() const
{
  return "PASSOUT";
}

bool ENDMANNINGTOTURNEND_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.ENDMANNINGTOTURNEND_IsLegal(i_PlayerName,i_StateName);
}

bool ENDMANNINGTOTURNEND_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.ENDMANNINGTOTURNEND_IsAuto(i_StateName);
}

bool ENDMANNINGTOTURNEND_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.ENDMANNINGTOTURNEND_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string ENDMANNINGTOTURNEND_Transition::GetName() const
{
  return "ENDMANNINGTOTURNEND";
}

bool LIKEOPTIONS_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.LIKEOPTIONS_IsLegal(i_PlayerName,i_StateName);
}

bool LIKEOPTIONS_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.LIKEOPTIONS_IsAuto(i_StateName);
}

bool LIKEOPTIONS_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.LIKEOPTIONS_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string LIKEOPTIONS_Transition::GetName() const
{
  return "LIKEOPTIONS";
}

bool OPTIONS_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.OPTIONS_IsLegal(i_PlayerName,i_StateName);
}

bool OPTIONS_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.OPTIONS_IsAuto(i_StateName);
}

bool OPTIONS_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.OPTIONS_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string OPTIONS_Transition::GetName() const
{
  return "OPTIONS";
}

bool ENDMANNINGTODISCARD_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.ENDMANNINGTODISCARD_IsLegal(i_PlayerName,i_StateName);
}

bool ENDMANNINGTODISCARD_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.ENDMANNINGTODISCARD_IsAuto(i_StateName);
}

bool ENDMANNINGTODISCARD_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.ENDMANNINGTODISCARD_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string ENDMANNINGTODISCARD_Transition::GetName() const
{
  return "ENDMANNINGTODISCARD";
}

bool BUYNEWCHEMFACTORIES_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.BUYNEWCHEMFACTORIES_IsLegal(i_PlayerName,i_StateName);
}

bool BUYNEWCHEMFACTORIES_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.BUYNEWCHEMFACTORIES_IsAuto(i_StateName);
}

bool BUYNEWCHEMFACTORIES_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.BUYNEWCHEMFACTORIES_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string BUYNEWCHEMFACTORIES_Transition::GetName() const
{
  return "BUYNEWCHEMFACTORIES";
}

bool STARTMANNING_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.STARTMANNING_IsLegal(i_PlayerName,i_StateName);
}

bool STARTMANNING_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.STARTMANNING_IsAuto(i_StateName);
}

bool STARTMANNING_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.STARTMANNING_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string STARTMANNING_Transition::GetName() const
{
  return "STARTMANNING";
}

bool BUYROBOTS_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.BUYROBOTS_IsLegal(i_PlayerName,i_StateName);
}

bool BUYROBOTS_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.BUYROBOTS_IsAuto(i_StateName);
}

bool BUYROBOTS_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.BUYROBOTS_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string BUYROBOTS_Transition::GetName() const
{
  return "BUYROBOTS";
}

bool INTERNALIZE_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.INTERNALIZE_IsLegal(i_PlayerName,i_StateName);
}

bool INTERNALIZE_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.INTERNALIZE_IsAuto(i_StateName);
}

bool INTERNALIZE_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.INTERNALIZE_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string INTERNALIZE_Transition::GetName() const
{
  return "INTERNALIZE";
}

bool ALTERMANNING_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.ALTERMANNING_IsLegal(i_PlayerName,i_StateName);
}

bool ALTERMANNING_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.ALTERMANNING_IsAuto(i_StateName);
}

bool ALTERMANNING_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.ALTERMANNING_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string ALTERMANNING_Transition::GetName() const
{
  return "ALTERMANNING";
}

bool ENDDISCARDTOPURCHASE_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.ENDDISCARDTOPURCHASE_IsLegal(i_PlayerName,i_StateName);
}

bool ENDDISCARDTOPURCHASE_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.ENDDISCARDTOPURCHASE_IsAuto(i_StateName);
}

bool ENDDISCARDTOPURCHASE_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.ENDDISCARDTOPURCHASE_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string ENDDISCARDTOPURCHASE_Transition::GetName() const
{
  return "ENDDISCARDTOPURCHASE";
}

bool BUYRESEARCHFACTORIES_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.BUYRESEARCHFACTORIES_IsLegal(i_PlayerName,i_StateName);
}

bool BUYRESEARCHFACTORIES_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.BUYRESEARCHFACTORIES_IsAuto(i_StateName);
}

bool BUYRESEARCHFACTORIES_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.BUYRESEARCHFACTORIES_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string BUYRESEARCHFACTORIES_Transition::GetName() const
{
  return "BUYRESEARCHFACTORIES";
}

bool FINALDONE_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.FINALDONE_IsLegal(i_PlayerName,i_StateName);
}

bool FINALDONE_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.FINALDONE_IsAuto(i_StateName);
}

bool FINALDONE_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.FINALDONE_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string FINALDONE_Transition::GetName() const
{
  return "FINALDONE";
}

bool WATERMULLIGAN_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.WATERMULLIGAN_IsLegal(i_PlayerName,i_StateName);
}

bool WATERMULLIGAN_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.WATERMULLIGAN_IsAuto(i_StateName);
}

bool WATERMULLIGAN_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.WATERMULLIGAN_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string WATERMULLIGAN_Transition::GetName() const
{
  return "WATERMULLIGAN";
}

bool INITCOMMODITY_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.INITCOMMODITY_IsLegal(i_PlayerName,i_StateName);
}

bool INITCOMMODITY_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.INITCOMMODITY_IsAuto(i_StateName);
}

bool INITCOMMODITY_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.INITCOMMODITY_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string INITCOMMODITY_Transition::GetName() const
{
  return "INITCOMMODITY";
}

bool STANDARDEXPERT_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.STANDARDEXPERT_IsLegal(i_PlayerName,i_StateName);
}

bool STANDARDEXPERT_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.STANDARDEXPERT_IsAuto(i_StateName);
}

bool STANDARDEXPERT_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.STANDARDEXPERT_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string STANDARDEXPERT_Transition::GetName() const
{
  return "STANDARDEXPERT";
}

bool BUYMEN_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.BUYMEN_IsLegal(i_PlayerName,i_StateName);
}

bool BUYMEN_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.BUYMEN_IsAuto(i_StateName);
}

bool BUYMEN_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.BUYMEN_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string BUYMEN_Transition::GetName() const
{
  return "BUYMEN";
}

bool PURCHASEDONE_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.PURCHASEDONE_IsLegal(i_PlayerName,i_StateName);
}

bool PURCHASEDONE_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.PURCHASEDONE_IsAuto(i_StateName);
}

bool PURCHASEDONE_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.PURCHASEDONE_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string PURCHASEDONE_Transition::GetName() const
{
  return "PURCHASEDONE";
}

bool GAMEOVER_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.GAMEOVER_IsLegal(i_PlayerName,i_StateName);
}

bool GAMEOVER_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.GAMEOVER_IsAuto(i_StateName);
}

bool GAMEOVER_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.GAMEOVER_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string GAMEOVER_Transition::GetName() const
{
  return "GAMEOVER";
}

bool REQUESTMEGA_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.REQUESTMEGA_IsLegal(i_PlayerName,i_StateName);
}

bool REQUESTMEGA_Transition::IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const
{
  return i_gi.REQUESTMEGA_IsAuto(i_StateName);
}

bool REQUESTMEGA_Transition::ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.REQUESTMEGA_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string REQUESTMEGA_Transition::GetName() const
{
  return "REQUESTMEGA";
}

OutpostStateMachine::OutpostStateMachine()
{
  StateType *pBid = new StateType("Bid","Player Bidding Turns");
  InsertState(pBid);
  StateType *pFinalManning = new StateType("FinalManning","Final Chance to Change Manning");
  InsertState(pFinalManning);
  StateType *pMegaResources = new StateType("MegaResources","Handle Mega Resource Request");
  InsertState(pMegaResources);
  StateType *pManualDiscard = new StateType("ManualDiscard","Handle Manual Discard");
  InsertState(pManualDiscard);
  StateType *pTurnEnd = new StateType("TurnEnd","End of Turn Handling");
  InsertState(pTurnEnd);
  StateType *pTerminalState = new StateType("TerminalState","End Of Game State");
  InsertState(pTerminalState);
  StateType *pTurnOrder = new StateType("TurnOrder","Turn Order Determination");
  InsertState(pTurnOrder);
  StateType *pInitialState = new StateType("InitialState","Initial Game State");
  InsertState(pInitialState);
  StateType *pStartResources = new StateType("StartResources","Start Resource Dissemination");
  InsertState(pStartResources);
  StateType *pDoDiscard = new StateType("DoDiscard","Handle Discards");
  InsertState(pDoDiscard);
  StateType *pSupplyFill = new StateType("SupplyFill","Supply Ship Fill");
  InsertState(pSupplyFill);
  StateType *pStartPurchase = new StateType("StartPurchase","Start Player Purchase and Bid Phase");
  InsertState(pStartPurchase);
  StateType *pValidateSetup = new StateType("ValidateSetup","Give all Players an Opportunity to ok Players and Options");
  InsertState(pValidateSetup);
  StateType *pPromptMegaResources = new StateType("PromptMegaResources","Prompt for Mega Resources");
  InsertState(pPromptMegaResources);
  StateType *pPurchase = new StateType("Purchase","Player Purchase Turns");
  InsertState(pPurchase);
  StateType *pStartDiscard = new StateType("StartDiscard","Start Discard Phase");
  InsertState(pStartDiscard);
  StateType *pStartFinalManning = new StateType("StartFinalManning","Start Final Chance to Change Manning");
  InsertState(pStartFinalManning);
  TransitionType *pPASS_Transition = new PASS_Transition;
  InsertTransition(pPASS_Transition);
  TransitionType *pFILLSHIP_Transition = new FILLSHIP_Transition;
  InsertTransition(pFILLSHIP_Transition);
  TransitionType *pJOIN_Transition = new JOIN_Transition;
  InsertTransition(pJOIN_Transition);
  TransitionType *pOPENBID_Transition = new OPENBID_Transition;
  InsertTransition(pOPENBID_Transition);
  TransitionType *pBUYOREFACTORIES_Transition = new BUYOREFACTORIES_Transition;
  InsertTransition(pBUYOREFACTORIES_Transition);
  TransitionType *pDONTLIKEOPTIONS_Transition = new DONTLIKEOPTIONS_Transition;
  InsertTransition(pDONTLIKEOPTIONS_Transition);
  TransitionType *pDISCARD_Transition = new DISCARD_Transition;
  InsertTransition(pDISCARD_Transition);
  TransitionType *pPURCHASEBID_Transition = new PURCHASEBID_Transition;
  InsertTransition(pPURCHASEBID_Transition);
  TransitionType *pAUTOSKIPDISCARD_Transition = new AUTOSKIPDISCARD_Transition;
  InsertTransition(pAUTOSKIPDISCARD_Transition);
  TransitionType *pBUYTITANIUMFACTORIES_Transition = new BUYTITANIUMFACTORIES_Transition;
  InsertTransition(pBUYTITANIUMFACTORIES_Transition);
  TransitionType *pDETERMINETURNORDER_Transition = new DETERMINETURNORDER_Transition;
  InsertTransition(pDETERMINETURNORDER_Transition);
  TransitionType *pDONEMANNING_Transition = new DONEMANNING_Transition;
  InsertTransition(pDONEMANNING_Transition);
  TransitionType *pVALIDATESETUP_Transition = new VALIDATESETUP_Transition;
  InsertTransition(pVALIDATESETUP_Transition);
  TransitionType *pBUYWATERFACTORIES_Transition = new BUYWATERFACTORIES_Transition;
  InsertTransition(pBUYWATERFACTORIES_Transition);
  TransitionType *pENDDISCARDTOTURNEND_Transition = new ENDDISCARDTOTURNEND_Transition;
  InsertTransition(pENDDISCARDTOTURNEND_Transition);
  TransitionType *pCOMMODITYDONETOPURCHASE_Transition = new COMMODITYDONETOPURCHASE_Transition;
  InsertTransition(pCOMMODITYDONETOPURCHASE_Transition);
  TransitionType *pUNJOIN_Transition = new UNJOIN_Transition;
  InsertTransition(pUNJOIN_Transition);
  TransitionType *pDONE_Transition = new DONE_Transition;
  InsertTransition(pDONE_Transition);
  TransitionType *pNEWTURN_Transition = new NEWTURN_Transition;
  InsertTransition(pNEWTURN_Transition);
  TransitionType *pSTARTGAME_Transition = new STARTGAME_Transition;
  InsertTransition(pSTARTGAME_Transition);
  TransitionType *pSTANDARDBASIC_Transition = new STANDARDBASIC_Transition;
  InsertTransition(pSTANDARDBASIC_Transition);
  TransitionType *pBID_Transition = new BID_Transition;
  InsertTransition(pBID_Transition);
  TransitionType *pSTARTPURCHASE_Transition = new STARTPURCHASE_Transition;
  InsertTransition(pSTARTPURCHASE_Transition);
  TransitionType *pCOMMODITYDONETODISCARD_Transition = new COMMODITYDONETODISCARD_Transition;
  InsertTransition(pCOMMODITYDONETODISCARD_Transition);
  TransitionType *pNUMMEGAS_Transition = new NUMMEGAS_Transition;
  InsertTransition(pNUMMEGAS_Transition);
  TransitionType *pSTARTDISCARD_Transition = new STARTDISCARD_Transition;
  InsertTransition(pSTARTDISCARD_Transition);
  TransitionType *pMANUALDISCARD_Transition = new MANUALDISCARD_Transition;
  InsertTransition(pMANUALDISCARD_Transition);
  TransitionType *pPASSOUT_Transition = new PASSOUT_Transition;
  InsertTransition(pPASSOUT_Transition);
  TransitionType *pENDMANNINGTOTURNEND_Transition = new ENDMANNINGTOTURNEND_Transition;
  InsertTransition(pENDMANNINGTOTURNEND_Transition);
  TransitionType *pLIKEOPTIONS_Transition = new LIKEOPTIONS_Transition;
  InsertTransition(pLIKEOPTIONS_Transition);
  TransitionType *pOPTIONS_Transition = new OPTIONS_Transition;
  InsertTransition(pOPTIONS_Transition);
  TransitionType *pENDMANNINGTODISCARD_Transition = new ENDMANNINGTODISCARD_Transition;
  InsertTransition(pENDMANNINGTODISCARD_Transition);
  TransitionType *pBUYNEWCHEMFACTORIES_Transition = new BUYNEWCHEMFACTORIES_Transition;
  InsertTransition(pBUYNEWCHEMFACTORIES_Transition);
  TransitionType *pSTARTMANNING_Transition = new STARTMANNING_Transition;
  InsertTransition(pSTARTMANNING_Transition);
  TransitionType *pBUYROBOTS_Transition = new BUYROBOTS_Transition;
  InsertTransition(pBUYROBOTS_Transition);
  TransitionType *pINTERNALIZE_Transition = new INTERNALIZE_Transition;
  InsertTransition(pINTERNALIZE_Transition);
  TransitionType *pALTERMANNING_Transition = new ALTERMANNING_Transition;
  InsertTransition(pALTERMANNING_Transition);
  TransitionType *pENDDISCARDTOPURCHASE_Transition = new ENDDISCARDTOPURCHASE_Transition;
  InsertTransition(pENDDISCARDTOPURCHASE_Transition);
  TransitionType *pBUYRESEARCHFACTORIES_Transition = new BUYRESEARCHFACTORIES_Transition;
  InsertTransition(pBUYRESEARCHFACTORIES_Transition);
  TransitionType *pFINALDONE_Transition = new FINALDONE_Transition;
  InsertTransition(pFINALDONE_Transition);
  TransitionType *pWATERMULLIGAN_Transition = new WATERMULLIGAN_Transition;
  InsertTransition(pWATERMULLIGAN_Transition);
  TransitionType *pINITCOMMODITY_Transition = new INITCOMMODITY_Transition;
  InsertTransition(pINITCOMMODITY_Transition);
  TransitionType *pSTANDARDEXPERT_Transition = new STANDARDEXPERT_Transition;
  InsertTransition(pSTANDARDEXPERT_Transition);
  TransitionType *pBUYMEN_Transition = new BUYMEN_Transition;
  InsertTransition(pBUYMEN_Transition);
  TransitionType *pPURCHASEDONE_Transition = new PURCHASEDONE_Transition;
  InsertTransition(pPURCHASEDONE_Transition);
  TransitionType *pGAMEOVER_Transition = new GAMEOVER_Transition;
  InsertTransition(pGAMEOVER_Transition);
  TransitionType *pREQUESTMEGA_Transition = new REQUESTMEGA_Transition;
  InsertTransition(pREQUESTMEGA_Transition);

  pBid->InsertTransition(*pPASS_Transition,*pBid);
  pSupplyFill->InsertTransition(*pFILLSHIP_Transition,*pStartResources);
  pInitialState->InsertTransition(*pJOIN_Transition,*pInitialState);
  pPurchase->InsertTransition(*pOPENBID_Transition,*pBid);
  pPurchase->InsertTransition(*pBUYOREFACTORIES_Transition,*pPurchase);
  pBid->InsertTransition(*pBUYOREFACTORIES_Transition,*pBid);
  pValidateSetup->InsertTransition(*pDONTLIKEOPTIONS_Transition,*pInitialState);
  pManualDiscard->InsertTransition(*pDISCARD_Transition,*pDoDiscard);
  pBid->InsertTransition(*pPURCHASEBID_Transition,*pPurchase);
  pDoDiscard->InsertTransition(*pAUTOSKIPDISCARD_Transition,*pDoDiscard);
  pPurchase->InsertTransition(*pBUYTITANIUMFACTORIES_Transition,*pPurchase);
  pBid->InsertTransition(*pBUYTITANIUMFACTORIES_Transition,*pBid);
  pTurnOrder->InsertTransition(*pDETERMINETURNORDER_Transition,*pSupplyFill);
  pFinalManning->InsertTransition(*pDONEMANNING_Transition,*pFinalManning);
  pInitialState->InsertTransition(*pVALIDATESETUP_Transition,*pValidateSetup);
  pPurchase->InsertTransition(*pBUYWATERFACTORIES_Transition,*pPurchase);
  pBid->InsertTransition(*pBUYWATERFACTORIES_Transition,*pBid);
  pDoDiscard->InsertTransition(*pENDDISCARDTOTURNEND_Transition,*pTurnEnd);
  pPromptMegaResources->InsertTransition(*pCOMMODITYDONETOPURCHASE_Transition,*pStartPurchase);
  pInitialState->InsertTransition(*pUNJOIN_Transition,*pInitialState);
  pPurchase->InsertTransition(*pDONE_Transition,*pPurchase);
  pBid->InsertTransition(*pDONE_Transition,*pBid);
  pTurnEnd->InsertTransition(*pNEWTURN_Transition,*pTurnOrder);
  pValidateSetup->InsertTransition(*pSTARTGAME_Transition,*pTurnOrder);
  pInitialState->InsertTransition(*pSTANDARDBASIC_Transition,*pInitialState);
  pBid->InsertTransition(*pBID_Transition,*pBid);
  pStartPurchase->InsertTransition(*pSTARTPURCHASE_Transition,*pPurchase);
  pPromptMegaResources->InsertTransition(*pCOMMODITYDONETODISCARD_Transition,*pStartDiscard);
  pMegaResources->InsertTransition(*pNUMMEGAS_Transition,*pPromptMegaResources);
  pStartDiscard->InsertTransition(*pSTARTDISCARD_Transition,*pDoDiscard);
  pDoDiscard->InsertTransition(*pMANUALDISCARD_Transition,*pManualDiscard);
  pBid->InsertTransition(*pPASSOUT_Transition,*pBid);
  pFinalManning->InsertTransition(*pENDMANNINGTOTURNEND_Transition,*pTurnEnd);
  pValidateSetup->InsertTransition(*pLIKEOPTIONS_Transition,*pValidateSetup);
  pInitialState->InsertTransition(*pOPTIONS_Transition,*pInitialState);
  pFinalManning->InsertTransition(*pENDMANNINGTODISCARD_Transition,*pStartDiscard);
  pPurchase->InsertTransition(*pBUYNEWCHEMFACTORIES_Transition,*pPurchase);
  pBid->InsertTransition(*pBUYNEWCHEMFACTORIES_Transition,*pBid);
  pStartFinalManning->InsertTransition(*pSTARTMANNING_Transition,*pFinalManning);
  pPurchase->InsertTransition(*pBUYROBOTS_Transition,*pPurchase);
  pBid->InsertTransition(*pBUYROBOTS_Transition,*pBid);
  pPurchase->InsertTransition(*pINTERNALIZE_Transition,*pPurchase);
  pBid->InsertTransition(*pINTERNALIZE_Transition,*pBid);
  pPurchase->InsertTransition(*pALTERMANNING_Transition,*pPurchase);
  pBid->InsertTransition(*pALTERMANNING_Transition,*pBid);
  pFinalManning->InsertTransition(*pALTERMANNING_Transition,*pFinalManning);
  pDoDiscard->InsertTransition(*pENDDISCARDTOPURCHASE_Transition,*pStartPurchase);
  pPurchase->InsertTransition(*pBUYRESEARCHFACTORIES_Transition,*pPurchase);
  pBid->InsertTransition(*pBUYRESEARCHFACTORIES_Transition,*pBid);
  pPurchase->InsertTransition(*pFINALDONE_Transition,*pPurchase);
  pBid->InsertTransition(*pFINALDONE_Transition,*pBid);
  pPurchase->InsertTransition(*pWATERMULLIGAN_Transition,*pPurchase);
  pBid->InsertTransition(*pWATERMULLIGAN_Transition,*pBid);
  pStartResources->InsertTransition(*pINITCOMMODITY_Transition,*pPromptMegaResources);
  pInitialState->InsertTransition(*pSTANDARDEXPERT_Transition,*pInitialState);
  pPurchase->InsertTransition(*pBUYMEN_Transition,*pPurchase);
  pBid->InsertTransition(*pBUYMEN_Transition,*pBid);
  pPurchase->InsertTransition(*pPURCHASEDONE_Transition,*pStartFinalManning);
  pTurnEnd->InsertTransition(*pGAMEOVER_Transition,*pTerminalState);
  pPromptMegaResources->InsertTransition(*pREQUESTMEGA_Transition,*pMegaResources);
}

OutpostSet *pGameSet;
OutpostStateMachine *pStateMachine;

bool Initialize(const std::string &i_DataDir)
{
  pStateMachine = new OutpostStateMachine;
  pGameSet = new OutpostSet(i_DataDir);
  if (!pGameSet || !pGameSet->IsValid())
  {
    return false;
  }
  return true;
}

Game *CreateGame(const ServerGameInfo &i_rServerGameInfo,OutputPort &i_rOutputPort)
{
  OutpostGame *pNewGame = new OutpostGame(i_rServerGameInfo,i_rOutputPort,
                                   *pGameSet,*pStateMachine,60);
  return pNewGame;
}
