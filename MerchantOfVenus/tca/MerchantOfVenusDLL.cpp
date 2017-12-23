#include <sstream>
#include <set>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include "StringUtilities.hpp"
#include "GameBoxDLL.hpp"
#include "MerchantOfVenusGameInfo.hpp"
#include "DLLGame.hpp"


    
      #define ERROUT(D_EXPR,D_ERRMSG) { if (D_EXPR) { UnicastMOVERROR(i_PlayerName,D_ERRMSG); return false; } }
      #include "Ship.hpp"
      #include "MapOverlay.hpp"

      // if i_playerid is empty, broadcast contents of trade bases to all players,
      // as well as secrecy status.
      // otherwise, unicast to that player id.
      void CastAllTradeBases(const MerchantOfVenusGameInfo& i_ogi,
                             const std::string& i_playername,const TradeBases& bases)
      {
        const std::map<std::string,std::string>& ssbyrace = bases.GetSolarSystemByRaceMap();
        const std::map<std::string,TradeBase>& basemap = bases.GetBaseMap();

        std::map<std::string,std::string>::const_iterator raceit;
        for (raceit = ssbyrace.begin() ; raceit != ssbyrace.end() ; ++raceit)
        {
          std::map<std::string,TradeBase>::const_iterator baseit = basemap.find(raceit->second);
          const TradeBase& base = baseit->second;

          const std::vector<Token>& bsell = base.GetSellableTokens();
          const std::vector<Token>& bdemand = base.GetDemandTokens();

          std::vector<Token>::const_iterator tit;
          for (tit = bsell.begin() ; tit != bsell.end() ; ++tit)
          {
            if (i_playername.size()==0) 
              i_ogi.BroadcastADDTOTRADEBASE(base.GetId(),tit->Encoding(),false);
            else 
              i_ogi.UnicastADDTOTRADEBASE(i_playername,base.GetId(),tit->Encoding(),false);
          }
          for (tit = bdemand.begin() ; tit != bdemand.end() ; ++tit)
          {
            if (i_playername.size()==0) 
              i_ogi.BroadcastADDTOTRADEBASE(base.GetId(),tit->Encoding(),false);
            else 
              i_ogi.UnicastADDTOTRADEBASE(i_playername,base.GetId(),tit->Encoding(),false);
          }
        }
      }

      // this method will inform the specified user about who knows what about all
      // inhabited solar systems, and will in addition, inform the player if they know
      // the race that inhabits.
      void UnicastTradeBaseSecrecy(const MerchantOfVenusGameInfo& i_ogi,
                                   const Players& i_players,
                                   const std::string& i_playername,
                                   const TradeBases& bases)
      {
        const std::map<std::string,TradeBase>& basemap = bases.GetBaseMap();
        std::map<std::string,TradeBase>::const_iterator baseit;
        for (baseit = basemap.begin() ; baseit != basemap.end() ; ++baseit)
        {
          const std::string& ssname = baseit->first;
          const TradeBase& base = baseit->second;
          const Secret& sec = base.GetSecret();
          i_ogi.UnicastSOLARSYSTEMPRIVACY(i_playername,ssname,sec.GetSecretString());
          if (sec.IsPublic() || (
                                 i_players.IsPlayer(i_playername) &&
                                 sec.PlayerKnowsSecret(i_players.GetPlayerId(i_playername))))
          {
            i_ogi.UnicastBASELOCATION(i_playername,ssname,base.GetId());
          }
        }
      }

      // this method will inform the given user about the secrecy of all
      // spaces that override the base map, and the identity of all spaces
      // they are privy to.
      void UnicastSpaceOverrides(const MerchantOfVenusGameInfo& i_ogi,
                                 const Players& i_players,
                                 std::string i_PlayerName,
                                 const MapOverlay& i_mo)
      {
        std::map<std::string,std::pair<MapSpace,Secret> >::const_iterator oit;
        for (oit = i_mo.GetOverrideMap().begin() ; oit != i_mo.GetOverrideMap().end() ; ++oit)
        {
          i_ogi.UnicastQBOXPRIVACY(i_PlayerName,oit->first,oit->second.second.GetSecretString());
          if (oit->second.second.IsPublic() ||
              (i_players.IsPlayer(i_PlayerName) &&
               oit->second.second.PlayerKnowsSecret(i_players.GetPlayerId(i_PlayerName))))
          {
            i_ogi.UnicastQBOXCONTENTS(i_PlayerName,oit->first,i_mo.GetSpaceString(oit->first));
          }
        }
      }
                         


      void ApplyStationCommission(const MerchantOfVenusGameInfo& i_ogi,
                                  Players& i_players,
                                  MapOverlay& i_mo,
                                  int amount)
      {
        const Player& curp = i_players.GetCurPlayer();
        // first, find who owns the station the current player is on (if anyone)
        std::map<std::string,size_t>::const_iterator fit = i_mo.GetStationOwners().find(curp.GetLocation());

        if (fit == i_mo.GetStationOwners().end()) return;

        // then give them the commission and tell everyone.
        size_t stationowner = fit->second;

        int commission = amount / 10;

        i_players[stationowner].AddMoney(commission);
        i_ogi.BroadcastUpdatePLAYERMONEYByplayerindex(stationowner);
      }

      void ApplyFactoryCommission(const MerchantOfVenusGameInfo& i_ogi,
                                  Players& i_players,
                                  MapOverlay& i_mo,
                                  int amount)
      {
        const Player& curp = i_players.GetCurPlayer();
        // first, find who owns the factory for the solar system the current player is in (if anyone)
        std::pair<MapSpace,Secret> curpair = i_mo.GetMapSpace(curp.GetLocation());
        const std::string& rname = curpair.first.m_regionname;
       
        // then see if anyone owns the factory (someone should, if the player just bought the factory good)
        std::map<std::string,size_t>::const_iterator fit = i_mo.GetFactoryOwners().find(rname);
        if (fit == i_mo.GetFactoryOwners().end()) return;

        // then give them the commission and tell everyone.
        size_t factoryowner = fit->second;
        int commission = amount / 2;

        i_players[factoryowner].AddMoney(commission);
        i_ogi.BroadcastUpdatePLAYERMONEYByplayerindex(factoryowner);
      }

      void DisposeOfToken(const MerchantOfVenusGameInfo& i_ogi,
                   TradeBases& i_bases,
                   TradeBase& i_curtradebase,
                   Cup& i_cup,
                   Token i_doomed)
      {
        i_doomed.MakeOld();
        switch(i_doomed.GetSellDestination())
        {
          case NOWHERE:
            break;
          case CULTURE:
            i_curtradebase.AddToken(i_doomed);
            i_ogi.BroadcastADDTOTRADEBASE(i_curtradebase.GetId(),i_doomed.Encoding(),false);
            break;
          case CUP:
            Token newt = i_cup.Replace(i_doomed);
            std::string ownerid = newt.GetSource();
            std::string newss = i_bases.GetSolarSystemOfRace(ownerid);
            TradeBase& newtb = i_bases.GetTradeBaseBySolarSystemName(newss);
            newtb.AddToken(newt);
            i_ogi.BroadcastADDTOTRADEBASE(newtb.GetId(),newt.Encoding(),true);
            break;
        }
      }

      int ProcessDemands(const MerchantOfVenusGameInfo& i_ogi,
                         TradeBase& i_tb,
                         TradeBases& i_bases,
                         Cup& i_cup,
                         Token sellitem)
      {
        // only non-factory goods have demands.
        if (sellitem.GetSizeClass() != GOOD_SIZE || sellitem.IsFactoryGood())
        {
          return sellitem.GetSellPrice();
        }
        int bonus = 0;
        size_t lastbonusidx = 0;
        for (size_t i = 0 ; i < i_tb.GetDemandTokens().size() ; ++i)
        {
          if (i_tb.GetDemandTokens()[i].GetName() == sellitem.GetName())
          {
            lastbonusidx = i;
            bonus += i_tb.GetDemandTokens()[i].GetSellPrice();
          }
        }
        if (bonus > 0)
        {
          Token demtoken = i_tb.GetDemandTokens()[lastbonusidx];
          i_ogi.BroadcastSUBTRACTFROMTRADEBASE(i_tb.GetId(),demtoken.Encoding());
          i_tb.RemoveDemandToken(lastbonusidx);
          DisposeOfToken(i_ogi,i_bases,i_tb,i_cup,demtoken);
        }
        return sellitem.GetSellPrice() - bonus;
      }

#define SENDTRANSCOUNT(PLN) SendTransCount(PLN,*this,GetMoveMediator(),GetMapOverlay(),GetPlayers())

      void SendTransCount(const std::string& i_PlayerName,
                          const MerchantOfVenusGameInfo& i_ogi,
                          const MoveMediator& i_mm,
                          const MapOverlay& i_mo,
                          const Players& i_pl)
      {
        const Player& curp = i_pl.GetCurPlayer();
        int numbuys;
        int numsells;

        if (i_mm.IsFirst() || i_mo.GetMapSpace(curp.GetLocation()).first.m_type == STATION) 
        {
          numbuys = -1;
          numsells = -1;
        }
        else
        {
          numbuys = 1 - curp.GetBuyCount();
          numsells = 1 - curp.GetSellCount();
        }

        if (i_PlayerName == "")
        {
          i_ogi.BroadcastREMAININGTRANSCOUNT(curp.GetId(),numbuys,numsells);
        }
        else
        {
          i_ogi.UnicastREMAININGTRANSCOUNT(i_PlayerName,curp.GetId(),numbuys,numsells);
        } 
      }

        

            

    
  
void MerchantOfVenusGameInfo::UnicastDESTINATION(const std::string &i_PlayerName,const int& i_curspacepenalty,const bool& i_isfirst,const std::string& i_destinations) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeDESTINATIONMessage(i_curspacepenalty,i_isfirst,i_destinations));
}
void MerchantOfVenusGameInfo::VaricastDESTINATION(const NameBoolean &i_Variator,const int& i_curspacepenalty,const bool& i_isfirst,const std::string& i_destinations) const
{
  m_OutputPort.VariCast(i_Variator,MakeDESTINATIONMessage(i_curspacepenalty,i_isfirst,i_destinations));
}
void MerchantOfVenusGameInfo::BroadcastDESTINATION(const int& i_curspacepenalty,const bool& i_isfirst,const std::string& i_destinations) const
{
  m_OutputPort.BroadCast(MakeDESTINATIONMessage(i_curspacepenalty,i_isfirst,i_destinations));
}

std::string MerchantOfVenusGameInfo::MakeDESTINATIONMessage(const int& i_curspacepenalty,const bool& i_isfirst,const std::string& i_destinations) const
{
  std::ostringstream oss;
  oss << "DESTINATION"
  << ',' << UnCommaStringify(i_curspacepenalty)
  << ',' << UnCommaStringify(i_isfirst)
  << ',' << UnCommaStringify(i_destinations)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastQBOXCONTENTS(const std::string &i_PlayerName,const std::string& i_spacename,const std::string& i_contents) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeQBOXCONTENTSMessage(i_spacename,i_contents));
}
void MerchantOfVenusGameInfo::VaricastQBOXCONTENTS(const NameBoolean &i_Variator,const std::string& i_spacename,const std::string& i_contents) const
{
  m_OutputPort.VariCast(i_Variator,MakeQBOXCONTENTSMessage(i_spacename,i_contents));
}
void MerchantOfVenusGameInfo::BroadcastQBOXCONTENTS(const std::string& i_spacename,const std::string& i_contents) const
{
  m_OutputPort.BroadCast(MakeQBOXCONTENTSMessage(i_spacename,i_contents));
}

std::string MerchantOfVenusGameInfo::MakeQBOXCONTENTSMessage(const std::string& i_spacename,const std::string& i_contents) const
{
  std::ostringstream oss;
  oss << "QBOXCONTENTS"
  << ',' << UnCommaStringify(i_spacename)
  << ',' << UnCommaStringify(i_contents)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastSUBTRACTFROMTRADEBASE(const std::string &i_PlayerName,const std::string& i_raceid,const std::string& i_token) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeSUBTRACTFROMTRADEBASEMessage(i_raceid,i_token));
}
void MerchantOfVenusGameInfo::VaricastSUBTRACTFROMTRADEBASE(const NameBoolean &i_Variator,const std::string& i_raceid,const std::string& i_token) const
{
  m_OutputPort.VariCast(i_Variator,MakeSUBTRACTFROMTRADEBASEMessage(i_raceid,i_token));
}
void MerchantOfVenusGameInfo::BroadcastSUBTRACTFROMTRADEBASE(const std::string& i_raceid,const std::string& i_token) const
{
  m_OutputPort.BroadCast(MakeSUBTRACTFROMTRADEBASEMessage(i_raceid,i_token));
}

std::string MerchantOfVenusGameInfo::MakeSUBTRACTFROMTRADEBASEMessage(const std::string& i_raceid,const std::string& i_token) const
{
  std::ostringstream oss;
  oss << "SUBTRACTFROMTRADEBASE"
  << ',' << UnCommaStringify(i_raceid)
  << ',' << UnCommaStringify(i_token)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastFACTORYOWNER(const std::string &i_PlayerName,const std::string& i_raceid,const size_t& i_playerid) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeFACTORYOWNERMessage(i_raceid,i_playerid));
}
void MerchantOfVenusGameInfo::VaricastFACTORYOWNER(const NameBoolean &i_Variator,const std::string& i_raceid,const size_t& i_playerid) const
{
  m_OutputPort.VariCast(i_Variator,MakeFACTORYOWNERMessage(i_raceid,i_playerid));
}
void MerchantOfVenusGameInfo::BroadcastFACTORYOWNER(const std::string& i_raceid,const size_t& i_playerid) const
{
  m_OutputPort.BroadCast(MakeFACTORYOWNERMessage(i_raceid,i_playerid));
}

std::string MerchantOfVenusGameInfo::MakeFACTORYOWNERMessage(const std::string& i_raceid,const size_t& i_playerid) const
{
  std::ostringstream oss;
  oss << "FACTORYOWNER"
  << ',' << UnCommaStringify(i_raceid)
  << ',' << UnCommaStringify(i_playerid)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastERROR(const std::string &i_PlayerName,const std::string& i_ErrorText) const
{
  ++m_errorcount;
  m_OutputPort.UniCast(i_PlayerName,MakeERRORMessage(i_ErrorText));
}
void MerchantOfVenusGameInfo::VaricastERROR(const NameBoolean &i_Variator,const std::string& i_ErrorText) const
{
  m_OutputPort.VariCast(i_Variator,MakeERRORMessage(i_ErrorText));
}
void MerchantOfVenusGameInfo::BroadcastERROR(const std::string& i_ErrorText) const
{
  m_OutputPort.BroadCast(MakeERRORMessage(i_ErrorText));
}

std::string MerchantOfVenusGameInfo::MakeERRORMessage(const std::string& i_ErrorText) const
{
  std::ostringstream oss;
  oss << "ERROR"
  << ',' << UnCommaStringify(i_ErrorText)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastCURPLAYER(const std::string &i_PlayerName) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeCURPLAYERMessage());
}
void MerchantOfVenusGameInfo::VaricastCURPLAYER(const NameBoolean &i_Variator) const
{
  m_OutputPort.VariCast(i_Variator,MakeCURPLAYERMessage());
}
void MerchantOfVenusGameInfo::BroadcastCURPLAYER() const
{
  m_OutputPort.BroadCast(MakeCURPLAYERMessage());
}

std::string MerchantOfVenusGameInfo::MakeCURPLAYERMessage() const
{
  std::ostringstream oss;
  oss << "CURPLAYER"
  << ',' << UnCommaStringify(GetPlayers().GetCurPlayer().GetId())  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastPLAYEROKSTARTUP(const std::string &i_PlayerName) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    UnicastUpdatePLAYEROKSTARTUPByplayerindex(i_PlayerName,playerindex);
  }
}
void MerchantOfVenusGameInfo::VaricastPLAYEROKSTARTUP(const NameBoolean &i_Variator) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    VaricastUpdatePLAYEROKSTARTUPByplayerindex(i_Variator,playerindex);
  }
}
void MerchantOfVenusGameInfo::BroadcastPLAYEROKSTARTUP() const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    BroadcastUpdatePLAYEROKSTARTUPByplayerindex(playerindex);
  }
}
void MerchantOfVenusGameInfo::UnicastUpdatePLAYEROKSTARTUPByplayerindex(const std::string &i_PlayerName,
      const size_t& playerindex) const
{
  m_OutputPort.UniCast(i_PlayerName,MakePLAYEROKSTARTUPMessage(playerindex));
}
void MerchantOfVenusGameInfo::VaricastUpdatePLAYEROKSTARTUPByplayerindex(const NameBoolean &i_Variator,
      const size_t& playerindex) const
{
  m_OutputPort.VariCast(i_Variator,MakePLAYEROKSTARTUPMessage(playerindex));
}
void MerchantOfVenusGameInfo::BroadcastUpdatePLAYEROKSTARTUPByplayerindex(
      const size_t& playerindex) const
{
  m_OutputPort.BroadCast(MakePLAYEROKSTARTUPMessage(playerindex));
}

std::string MerchantOfVenusGameInfo::MakePLAYEROKSTARTUPMessage(const size_t& playerindex) const
{
  std::ostringstream oss;
  oss << "PLAYEROKSTARTUP"
  << ',' << UnCommaStringify(playerindex)
  << ',' << UnCommaStringify(GetPlayers()[playerindex].OkStartup())  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastMESSAGE(const std::string &i_PlayerName,const std::string& i_MessageText) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeMESSAGEMessage(i_MessageText));
}
void MerchantOfVenusGameInfo::VaricastMESSAGE(const NameBoolean &i_Variator,const std::string& i_MessageText) const
{
  m_OutputPort.VariCast(i_Variator,MakeMESSAGEMessage(i_MessageText));
}
void MerchantOfVenusGameInfo::BroadcastMESSAGE(const std::string& i_MessageText) const
{
  m_OutputPort.BroadCast(MakeMESSAGEMessage(i_MessageText));
}

std::string MerchantOfVenusGameInfo::MakeMESSAGEMessage(const std::string& i_MessageText) const
{
  std::ostringstream oss;
  oss << "MESSAGE"
  << ',' << UnCommaStringify(i_MessageText)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastSWITCHABLES(const std::string &i_PlayerName,const std::string& i_switchables) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeSWITCHABLESMessage(i_switchables));
}
void MerchantOfVenusGameInfo::VaricastSWITCHABLES(const NameBoolean &i_Variator,const std::string& i_switchables) const
{
  m_OutputPort.VariCast(i_Variator,MakeSWITCHABLESMessage(i_switchables));
}
void MerchantOfVenusGameInfo::BroadcastSWITCHABLES(const std::string& i_switchables) const
{
  m_OutputPort.BroadCast(MakeSWITCHABLESMessage(i_switchables));
}

std::string MerchantOfVenusGameInfo::MakeSWITCHABLESMessage(const std::string& i_switchables) const
{
  std::ostringstream oss;
  oss << "SWITCHABLES"
  << ',' << UnCommaStringify(i_switchables)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastSELECTEDSWITCHABLES(const std::string &i_PlayerName,const size_t& i_playerid,const std::string& i_switchables) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeSELECTEDSWITCHABLESMessage(i_playerid,i_switchables));
}
void MerchantOfVenusGameInfo::VaricastSELECTEDSWITCHABLES(const NameBoolean &i_Variator,const size_t& i_playerid,const std::string& i_switchables) const
{
  m_OutputPort.VariCast(i_Variator,MakeSELECTEDSWITCHABLESMessage(i_playerid,i_switchables));
}
void MerchantOfVenusGameInfo::BroadcastSELECTEDSWITCHABLES(const size_t& i_playerid,const std::string& i_switchables) const
{
  m_OutputPort.BroadCast(MakeSELECTEDSWITCHABLESMessage(i_playerid,i_switchables));
}

std::string MerchantOfVenusGameInfo::MakeSELECTEDSWITCHABLESMessage(const size_t& i_playerid,const std::string& i_switchables) const
{
  std::ostringstream oss;
  oss << "SELECTEDSWITCHABLES"
  << ',' << UnCommaStringify(i_playerid)
  << ',' << UnCommaStringify(i_switchables)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastADDTOTRADEBASE(const std::string &i_PlayerName,const std::string& i_raceid,const std::string& i_token,const bool& i_fromcup) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeADDTOTRADEBASEMessage(i_raceid,i_token,i_fromcup));
}
void MerchantOfVenusGameInfo::VaricastADDTOTRADEBASE(const NameBoolean &i_Variator,const std::string& i_raceid,const std::string& i_token,const bool& i_fromcup) const
{
  m_OutputPort.VariCast(i_Variator,MakeADDTOTRADEBASEMessage(i_raceid,i_token,i_fromcup));
}
void MerchantOfVenusGameInfo::BroadcastADDTOTRADEBASE(const std::string& i_raceid,const std::string& i_token,const bool& i_fromcup) const
{
  m_OutputPort.BroadCast(MakeADDTOTRADEBASEMessage(i_raceid,i_token,i_fromcup));
}

std::string MerchantOfVenusGameInfo::MakeADDTOTRADEBASEMessage(const std::string& i_raceid,const std::string& i_token,const bool& i_fromcup) const
{
  std::ostringstream oss;
  oss << "ADDTOTRADEBASE"
  << ',' << UnCommaStringify(i_raceid)
  << ',' << UnCommaStringify(i_token)
  << ',' << UnCommaStringify(i_fromcup)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastMOVERROR(const std::string &i_PlayerName,const std::string& i_Error) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeMOVERRORMessage(i_Error));
}
void MerchantOfVenusGameInfo::VaricastMOVERROR(const NameBoolean &i_Variator,const std::string& i_Error) const
{
  m_OutputPort.VariCast(i_Variator,MakeMOVERRORMessage(i_Error));
}
void MerchantOfVenusGameInfo::BroadcastMOVERROR(const std::string& i_Error) const
{
  m_OutputPort.BroadCast(MakeMOVERRORMessage(i_Error));
}

std::string MerchantOfVenusGameInfo::MakeMOVERRORMessage(const std::string& i_Error) const
{
  std::ostringstream oss;
  oss << "MOVERROR"
  << ',' << UnCommaStringify(i_Error)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastTURNORDER(const std::string &i_PlayerName) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeTURNORDERMessage());
}
void MerchantOfVenusGameInfo::VaricastTURNORDER(const NameBoolean &i_Variator) const
{
  m_OutputPort.VariCast(i_Variator,MakeTURNORDERMessage());
}
void MerchantOfVenusGameInfo::BroadcastTURNORDER() const
{
  m_OutputPort.BroadCast(MakeTURNORDERMessage());
}

std::string MerchantOfVenusGameInfo::MakeTURNORDERMessage() const
{
  std::ostringstream oss;
  oss << "TURNORDER"
  << ',' << UnCommaStringify(GetPlayers().GetTurnOrder())  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastPLAYERLOCATION(const std::string &i_PlayerName) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    UnicastUpdatePLAYERLOCATIONByplayerindex(i_PlayerName,playerindex);
  }
}
void MerchantOfVenusGameInfo::VaricastPLAYERLOCATION(const NameBoolean &i_Variator) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    VaricastUpdatePLAYERLOCATIONByplayerindex(i_Variator,playerindex);
  }
}
void MerchantOfVenusGameInfo::BroadcastPLAYERLOCATION() const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    BroadcastUpdatePLAYERLOCATIONByplayerindex(playerindex);
  }
}
void MerchantOfVenusGameInfo::UnicastUpdatePLAYERLOCATIONByplayerindex(const std::string &i_PlayerName,
      const size_t& playerindex) const
{
  m_OutputPort.UniCast(i_PlayerName,MakePLAYERLOCATIONMessage(playerindex));
}
void MerchantOfVenusGameInfo::VaricastUpdatePLAYERLOCATIONByplayerindex(const NameBoolean &i_Variator,
      const size_t& playerindex) const
{
  m_OutputPort.VariCast(i_Variator,MakePLAYERLOCATIONMessage(playerindex));
}
void MerchantOfVenusGameInfo::BroadcastUpdatePLAYERLOCATIONByplayerindex(
      const size_t& playerindex) const
{
  m_OutputPort.BroadCast(MakePLAYERLOCATIONMessage(playerindex));
}

std::string MerchantOfVenusGameInfo::MakePLAYERLOCATIONMessage(const size_t& playerindex) const
{
  std::ostringstream oss;
  oss << "PLAYERLOCATION"
  << ',' << UnCommaStringify(playerindex)
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetLocation())  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastSUBTRACTFROMPLAYER(const std::string &i_PlayerName,const size_t& i_playerid,const std::string& i_token) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeSUBTRACTFROMPLAYERMessage(i_playerid,i_token));
}
void MerchantOfVenusGameInfo::VaricastSUBTRACTFROMPLAYER(const NameBoolean &i_Variator,const size_t& i_playerid,const std::string& i_token) const
{
  m_OutputPort.VariCast(i_Variator,MakeSUBTRACTFROMPLAYERMessage(i_playerid,i_token));
}
void MerchantOfVenusGameInfo::BroadcastSUBTRACTFROMPLAYER(const size_t& i_playerid,const std::string& i_token) const
{
  m_OutputPort.BroadCast(MakeSUBTRACTFROMPLAYERMessage(i_playerid,i_token));
}

std::string MerchantOfVenusGameInfo::MakeSUBTRACTFROMPLAYERMessage(const size_t& i_playerid,const std::string& i_token) const
{
  std::ostringstream oss;
  oss << "SUBTRACTFROMPLAYER"
  << ',' << UnCommaStringify(i_playerid)
  << ',' << UnCommaStringify(i_token)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastDICE(const std::string &i_PlayerName,const std::string& i_dicestring) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeDICEMessage(i_dicestring));
}
void MerchantOfVenusGameInfo::VaricastDICE(const NameBoolean &i_Variator,const std::string& i_dicestring) const
{
  m_OutputPort.VariCast(i_Variator,MakeDICEMessage(i_dicestring));
}
void MerchantOfVenusGameInfo::BroadcastDICE(const std::string& i_dicestring) const
{
  m_OutputPort.BroadCast(MakeDICEMessage(i_dicestring));
}

std::string MerchantOfVenusGameInfo::MakeDICEMessage(const std::string& i_dicestring) const
{
  std::ostringstream oss;
  oss << "DICE"
  << ',' << UnCommaStringify(i_dicestring)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastREMAININGTRANSCOUNT(const std::string &i_PlayerName,const size_t& i_playerid,const int& numbuys,const int& numsells) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeREMAININGTRANSCOUNTMessage(i_playerid,numbuys,numsells));
}
void MerchantOfVenusGameInfo::VaricastREMAININGTRANSCOUNT(const NameBoolean &i_Variator,const size_t& i_playerid,const int& numbuys,const int& numsells) const
{
  m_OutputPort.VariCast(i_Variator,MakeREMAININGTRANSCOUNTMessage(i_playerid,numbuys,numsells));
}
void MerchantOfVenusGameInfo::BroadcastREMAININGTRANSCOUNT(const size_t& i_playerid,const int& numbuys,const int& numsells) const
{
  m_OutputPort.BroadCast(MakeREMAININGTRANSCOUNTMessage(i_playerid,numbuys,numsells));
}

std::string MerchantOfVenusGameInfo::MakeREMAININGTRANSCOUNTMessage(const size_t& i_playerid,const int& numbuys,const int& numsells) const
{
  std::ostringstream oss;
  oss << "REMAININGTRANSCOUNT"
  << ',' << UnCommaStringify(i_playerid)
  << ',' << UnCommaStringify(numbuys)
  << ',' << UnCommaStringify(numsells)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastPLAYERRACE(const std::string &i_PlayerName) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    UnicastUpdatePLAYERRACEByplayerindex(i_PlayerName,playerindex);
  }
}
void MerchantOfVenusGameInfo::VaricastPLAYERRACE(const NameBoolean &i_Variator) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    VaricastUpdatePLAYERRACEByplayerindex(i_Variator,playerindex);
  }
}
void MerchantOfVenusGameInfo::BroadcastPLAYERRACE() const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    BroadcastUpdatePLAYERRACEByplayerindex(playerindex);
  }
}
void MerchantOfVenusGameInfo::UnicastUpdatePLAYERRACEByplayerindex(const std::string &i_PlayerName,
      const size_t& playerindex) const
{
  m_OutputPort.UniCast(i_PlayerName,MakePLAYERRACEMessage(playerindex));
}
void MerchantOfVenusGameInfo::VaricastUpdatePLAYERRACEByplayerindex(const NameBoolean &i_Variator,
      const size_t& playerindex) const
{
  m_OutputPort.VariCast(i_Variator,MakePLAYERRACEMessage(playerindex));
}
void MerchantOfVenusGameInfo::BroadcastUpdatePLAYERRACEByplayerindex(
      const size_t& playerindex) const
{
  m_OutputPort.BroadCast(MakePLAYERRACEMessage(playerindex));
}

std::string MerchantOfVenusGameInfo::MakePLAYERRACEMessage(const size_t& playerindex) const
{
  std::ostringstream oss;
  oss << "PLAYERRACE"
  << ',' << UnCommaStringify(playerindex)
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetRace())  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastPLAYER(const std::string &i_PlayerName) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    UnicastUpdatePLAYERByplayerindex(i_PlayerName,playerindex);
  }
}
void MerchantOfVenusGameInfo::VaricastPLAYER(const NameBoolean &i_Variator) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    VaricastUpdatePLAYERByplayerindex(i_Variator,playerindex);
  }
}
void MerchantOfVenusGameInfo::BroadcastPLAYER() const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    BroadcastUpdatePLAYERByplayerindex(playerindex);
  }
}
void MerchantOfVenusGameInfo::UnicastUpdatePLAYERByplayerindex(const std::string &i_PlayerName,
      const size_t& playerindex) const
{
  m_OutputPort.UniCast(i_PlayerName,MakePLAYERMessage(playerindex));
}
void MerchantOfVenusGameInfo::VaricastUpdatePLAYERByplayerindex(const NameBoolean &i_Variator,
      const size_t& playerindex) const
{
  m_OutputPort.VariCast(i_Variator,MakePLAYERMessage(playerindex));
}
void MerchantOfVenusGameInfo::BroadcastUpdatePLAYERByplayerindex(
      const size_t& playerindex) const
{
  m_OutputPort.BroadCast(MakePLAYERMessage(playerindex));
}

std::string MerchantOfVenusGameInfo::MakePLAYERMessage(const size_t& playerindex) const
{
  std::ostringstream oss;
  oss << "PLAYER"
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetName())
  << ',' << UnCommaStringify(playerindex)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastMPLEFT(const std::string &i_PlayerName,const int& i_mpleft) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeMPLEFTMessage(i_mpleft));
}
void MerchantOfVenusGameInfo::VaricastMPLEFT(const NameBoolean &i_Variator,const int& i_mpleft) const
{
  m_OutputPort.VariCast(i_Variator,MakeMPLEFTMessage(i_mpleft));
}
void MerchantOfVenusGameInfo::BroadcastMPLEFT(const int& i_mpleft) const
{
  m_OutputPort.BroadCast(MakeMPLEFTMessage(i_mpleft));
}

std::string MerchantOfVenusGameInfo::MakeMPLEFTMessage(const int& i_mpleft) const
{
  std::ostringstream oss;
  oss << "MPLEFT"
  << ',' << UnCommaStringify(i_mpleft)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastPILOTNUMBERCHOSEN(const std::string &i_PlayerName,const size_t& i_playerid,const size_t& i_pilotnumber,const bool& i_ispermanent) const
{
  m_OutputPort.UniCast(i_PlayerName,MakePILOTNUMBERCHOSENMessage(i_playerid,i_pilotnumber,i_ispermanent));
}
void MerchantOfVenusGameInfo::VaricastPILOTNUMBERCHOSEN(const NameBoolean &i_Variator,const size_t& i_playerid,const size_t& i_pilotnumber,const bool& i_ispermanent) const
{
  m_OutputPort.VariCast(i_Variator,MakePILOTNUMBERCHOSENMessage(i_playerid,i_pilotnumber,i_ispermanent));
}
void MerchantOfVenusGameInfo::BroadcastPILOTNUMBERCHOSEN(const size_t& i_playerid,const size_t& i_pilotnumber,const bool& i_ispermanent) const
{
  m_OutputPort.BroadCast(MakePILOTNUMBERCHOSENMessage(i_playerid,i_pilotnumber,i_ispermanent));
}

std::string MerchantOfVenusGameInfo::MakePILOTNUMBERCHOSENMessage(const size_t& i_playerid,const size_t& i_pilotnumber,const bool& i_ispermanent) const
{
  std::ostringstream oss;
  oss << "PILOTNUMBERCHOSEN"
  << ',' << UnCommaStringify(i_playerid)
  << ',' << UnCommaStringify(i_pilotnumber)
  << ',' << UnCommaStringify(i_ispermanent)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastSOLARSYSTEMPRIVACY(const std::string &i_PlayerName,const std::string& i_solarsystem,const std::string& i_secrecy) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeSOLARSYSTEMPRIVACYMessage(i_solarsystem,i_secrecy));
}
void MerchantOfVenusGameInfo::VaricastSOLARSYSTEMPRIVACY(const NameBoolean &i_Variator,const std::string& i_solarsystem,const std::string& i_secrecy) const
{
  m_OutputPort.VariCast(i_Variator,MakeSOLARSYSTEMPRIVACYMessage(i_solarsystem,i_secrecy));
}
void MerchantOfVenusGameInfo::BroadcastSOLARSYSTEMPRIVACY(const std::string& i_solarsystem,const std::string& i_secrecy) const
{
  m_OutputPort.BroadCast(MakeSOLARSYSTEMPRIVACYMessage(i_solarsystem,i_secrecy));
}

std::string MerchantOfVenusGameInfo::MakeSOLARSYSTEMPRIVACYMessage(const std::string& i_solarsystem,const std::string& i_secrecy) const
{
  std::ostringstream oss;
  oss << "SOLARSYSTEMPRIVACY"
  << ',' << UnCommaStringify(i_solarsystem)
  << ',' << UnCommaStringify(i_secrecy)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastOPTIONS(const std::string &i_PlayerName) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeOPTIONSMessage());
}
void MerchantOfVenusGameInfo::VaricastOPTIONS(const NameBoolean &i_Variator) const
{
  m_OutputPort.VariCast(i_Variator,MakeOPTIONSMessage());
}
void MerchantOfVenusGameInfo::BroadcastOPTIONS() const
{
  m_OutputPort.BroadCast(MakeOPTIONSMessage());
}

std::string MerchantOfVenusGameInfo::MakeOPTIONSMessage() const
{
  std::ostringstream oss;
  oss << "OPTIONS"
  << ',' << UnCommaStringify(m_winmoney)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastPLAYERWINS(const std::string &i_PlayerName,const size_t& i_playerid) const
{
  m_OutputPort.UniCast(i_PlayerName,MakePLAYERWINSMessage(i_playerid));
}
void MerchantOfVenusGameInfo::VaricastPLAYERWINS(const NameBoolean &i_Variator,const size_t& i_playerid) const
{
  m_OutputPort.VariCast(i_Variator,MakePLAYERWINSMessage(i_playerid));
}
void MerchantOfVenusGameInfo::BroadcastPLAYERWINS(const size_t& i_playerid) const
{
  m_OutputPort.BroadCast(MakePLAYERWINSMessage(i_playerid));
}

std::string MerchantOfVenusGameInfo::MakePLAYERWINSMessage(const size_t& i_playerid) const
{
  std::ostringstream oss;
  oss << "PLAYERWINS"
  << ',' << UnCommaStringify(i_playerid)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastLEGALACTION(const std::string &i_PlayerName,const std::string& i_ActionName) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeLEGALACTIONMessage(i_ActionName));
}
void MerchantOfVenusGameInfo::VaricastLEGALACTION(const NameBoolean &i_Variator,const std::string& i_ActionName) const
{
  m_OutputPort.VariCast(i_Variator,MakeLEGALACTIONMessage(i_ActionName));
}
void MerchantOfVenusGameInfo::BroadcastLEGALACTION(const std::string& i_ActionName) const
{
  m_OutputPort.BroadCast(MakeLEGALACTIONMessage(i_ActionName));
}

std::string MerchantOfVenusGameInfo::MakeLEGALACTIONMessage(const std::string& i_ActionName) const
{
  std::ostringstream oss;
  oss << "LEGALACTION"
  << ',' << UnCommaStringify(i_ActionName)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastADDTOPLAYER(const std::string &i_PlayerName,const size_t& i_playerid,const std::string& i_token) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeADDTOPLAYERMessage(i_playerid,i_token));
}
void MerchantOfVenusGameInfo::VaricastADDTOPLAYER(const NameBoolean &i_Variator,const size_t& i_playerid,const std::string& i_token) const
{
  m_OutputPort.VariCast(i_Variator,MakeADDTOPLAYERMessage(i_playerid,i_token));
}
void MerchantOfVenusGameInfo::BroadcastADDTOPLAYER(const size_t& i_playerid,const std::string& i_token) const
{
  m_OutputPort.BroadCast(MakeADDTOPLAYERMessage(i_playerid,i_token));
}

std::string MerchantOfVenusGameInfo::MakeADDTOPLAYERMessage(const size_t& i_playerid,const std::string& i_token) const
{
  std::ostringstream oss;
  oss << "ADDTOPLAYER"
  << ',' << UnCommaStringify(i_playerid)
  << ',' << UnCommaStringify(i_token)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastQBOXPRIVACY(const std::string &i_PlayerName,const std::string& i_spacename,const std::string& i_secrecy) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeQBOXPRIVACYMessage(i_spacename,i_secrecy));
}
void MerchantOfVenusGameInfo::VaricastQBOXPRIVACY(const NameBoolean &i_Variator,const std::string& i_spacename,const std::string& i_secrecy) const
{
  m_OutputPort.VariCast(i_Variator,MakeQBOXPRIVACYMessage(i_spacename,i_secrecy));
}
void MerchantOfVenusGameInfo::BroadcastQBOXPRIVACY(const std::string& i_spacename,const std::string& i_secrecy) const
{
  m_OutputPort.BroadCast(MakeQBOXPRIVACYMessage(i_spacename,i_secrecy));
}

std::string MerchantOfVenusGameInfo::MakeQBOXPRIVACYMessage(const std::string& i_spacename,const std::string& i_secrecy) const
{
  std::ostringstream oss;
  oss << "QBOXPRIVACY"
  << ',' << UnCommaStringify(i_spacename)
  << ',' << UnCommaStringify(i_secrecy)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastPILOTNUMBERCHOICE(const std::string &i_PlayerName,const std::string& i_pnumbers) const
{
  m_OutputPort.UniCast(i_PlayerName,MakePILOTNUMBERCHOICEMessage(i_pnumbers));
}
void MerchantOfVenusGameInfo::VaricastPILOTNUMBERCHOICE(const NameBoolean &i_Variator,const std::string& i_pnumbers) const
{
  m_OutputPort.VariCast(i_Variator,MakePILOTNUMBERCHOICEMessage(i_pnumbers));
}
void MerchantOfVenusGameInfo::BroadcastPILOTNUMBERCHOICE(const std::string& i_pnumbers) const
{
  m_OutputPort.BroadCast(MakePILOTNUMBERCHOICEMessage(i_pnumbers));
}

std::string MerchantOfVenusGameInfo::MakePILOTNUMBERCHOICEMessage(const std::string& i_pnumbers) const
{
  std::ostringstream oss;
  oss << "PILOTNUMBERCHOICE"
  << ',' << UnCommaStringify(i_pnumbers)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastBASELOCATION(const std::string &i_PlayerName,const std::string& i_solarsystem,const std::string& i_raceid) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeBASELOCATIONMessage(i_solarsystem,i_raceid));
}
void MerchantOfVenusGameInfo::VaricastBASELOCATION(const NameBoolean &i_Variator,const std::string& i_solarsystem,const std::string& i_raceid) const
{
  m_OutputPort.VariCast(i_Variator,MakeBASELOCATIONMessage(i_solarsystem,i_raceid));
}
void MerchantOfVenusGameInfo::BroadcastBASELOCATION(const std::string& i_solarsystem,const std::string& i_raceid) const
{
  m_OutputPort.BroadCast(MakeBASELOCATIONMessage(i_solarsystem,i_raceid));
}

std::string MerchantOfVenusGameInfo::MakeBASELOCATIONMessage(const std::string& i_solarsystem,const std::string& i_raceid) const
{
  std::ostringstream oss;
  oss << "BASELOCATION"
  << ',' << UnCommaStringify(i_solarsystem)
  << ',' << UnCommaStringify(i_raceid)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastPLAYERMONEY(const std::string &i_PlayerName) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    UnicastUpdatePLAYERMONEYByplayerindex(i_PlayerName,playerindex);
  }
}
void MerchantOfVenusGameInfo::VaricastPLAYERMONEY(const NameBoolean &i_Variator) const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    VaricastUpdatePLAYERMONEYByplayerindex(i_Variator,playerindex);
  }
}
void MerchantOfVenusGameInfo::BroadcastPLAYERMONEY() const
{
  for( size_t playerindex = 0 ; playerindex != GetPlayers().size() ; ++playerindex ) {
    BroadcastUpdatePLAYERMONEYByplayerindex(playerindex);
  }
}
void MerchantOfVenusGameInfo::UnicastUpdatePLAYERMONEYByplayerindex(const std::string &i_PlayerName,
      const size_t& playerindex) const
{
  m_OutputPort.UniCast(i_PlayerName,MakePLAYERMONEYMessage(playerindex));
}
void MerchantOfVenusGameInfo::VaricastUpdatePLAYERMONEYByplayerindex(const NameBoolean &i_Variator,
      const size_t& playerindex) const
{
  m_OutputPort.VariCast(i_Variator,MakePLAYERMONEYMessage(playerindex));
}
void MerchantOfVenusGameInfo::BroadcastUpdatePLAYERMONEYByplayerindex(
      const size_t& playerindex) const
{
  m_OutputPort.BroadCast(MakePLAYERMONEYMessage(playerindex));
}

std::string MerchantOfVenusGameInfo::MakePLAYERMONEYMessage(const size_t& playerindex) const
{
  std::ostringstream oss;
  oss << "PLAYERMONEY"
  << ',' << UnCommaStringify(playerindex)
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetMoney())
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetBarter())
  << ',' << UnCommaStringify(GetPlayers()[playerindex].GetNetWorth())  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastAGEPLAYER(const std::string &i_PlayerName,const size_t& i_playerid) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeAGEPLAYERMessage(i_playerid));
}
void MerchantOfVenusGameInfo::VaricastAGEPLAYER(const NameBoolean &i_Variator,const size_t& i_playerid) const
{
  m_OutputPort.VariCast(i_Variator,MakeAGEPLAYERMessage(i_playerid));
}
void MerchantOfVenusGameInfo::BroadcastAGEPLAYER(const size_t& i_playerid) const
{
  m_OutputPort.BroadCast(MakeAGEPLAYERMessage(i_playerid));
}

std::string MerchantOfVenusGameInfo::MakeAGEPLAYERMessage(const size_t& i_playerid) const
{
  std::ostringstream oss;
  oss << "AGEPLAYER"
  << ',' << UnCommaStringify(i_playerid)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastRESET(const std::string &i_PlayerName) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeRESETMessage());
}
void MerchantOfVenusGameInfo::VaricastRESET(const NameBoolean &i_Variator) const
{
  m_OutputPort.VariCast(i_Variator,MakeRESETMessage());
}
void MerchantOfVenusGameInfo::BroadcastRESET() const
{
  m_OutputPort.BroadCast(MakeRESETMessage());
}

std::string MerchantOfVenusGameInfo::MakeRESETMessage() const
{
  std::ostringstream oss;
  oss << "RESET"
  << ',' << UnCommaStringify(m_ServerGameInfo.GetName())  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastIAM(const std::string &i_PlayerName,const size_t& i_PId) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeIAMMessage(i_PId));
}
void MerchantOfVenusGameInfo::VaricastIAM(const NameBoolean &i_Variator,const size_t& i_PId) const
{
  m_OutputPort.VariCast(i_Variator,MakeIAMMessage(i_PId));
}
void MerchantOfVenusGameInfo::BroadcastIAM(const size_t& i_PId) const
{
  m_OutputPort.BroadCast(MakeIAMMessage(i_PId));
}

std::string MerchantOfVenusGameInfo::MakeIAMMessage(const size_t& i_PId) const
{
  std::ostringstream oss;
  oss << "IAM"
  << ',' << UnCommaStringify(i_PId)  ;
  return oss.str();
}

void MerchantOfVenusGameInfo::UnicastNEWSTATE(const std::string &i_PlayerName,const std::string& i_EventName,const std::string& i_EventDesc) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeNEWSTATEMessage(i_EventName,i_EventDesc));
}
void MerchantOfVenusGameInfo::VaricastNEWSTATE(const NameBoolean &i_Variator,const std::string& i_EventName,const std::string& i_EventDesc) const
{
  m_OutputPort.VariCast(i_Variator,MakeNEWSTATEMessage(i_EventName,i_EventDesc));
}
void MerchantOfVenusGameInfo::BroadcastNEWSTATE(const std::string& i_EventName,const std::string& i_EventDesc) const
{
  m_OutputPort.BroadCast(MakeNEWSTATEMessage(i_EventName,i_EventDesc));
}

std::string MerchantOfVenusGameInfo::MakeNEWSTATEMessage(const std::string& i_EventName,const std::string& i_EventDesc) const
{
  std::ostringstream oss;
  oss << "NEWSTATE"
  << ',' << UnCommaStringify(i_EventName)
  << ',' << UnCommaStringify(i_EventDesc)  ;
  return oss.str();
}


MerchantOfVenusGameInfo::MerchantOfVenusGameInfo(const MerchantOfVenusSet &i_Set,
                                 const ServerGameInfo &i_ginfo,
                                 OutputPort  &i_Port) :
  GameInfo<MerchantOfVenusSet,MerchantOfVenusState>(i_Set,i_ginfo,i_Port) {}

void MerchantOfVenusGameInfo::ResetERRORCount() { m_errorcount = 0; }
int MerchantOfVenusGameInfo::GetERRORCount() const { return m_errorcount; }
bool MerchantOfVenusGameInfo::JOIN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::InitialState
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

bool MerchantOfVenusGameInfo::JOIN_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "JOIN";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::JOIN_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::InitialState
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
      UnicastIAM(i_PlayerName,GetPlayers().size()-1);
      BroadcastUpdatePLAYERByplayerindex(GetPlayers().size()-1);
      return true;
    ;
}

bool MerchantOfVenusGameInfo::AUTOPILOT_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::ProcessPilotNumber
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "AUTOPILOT";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::AUTOPILOT_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::ProcessPilotNumber
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "AUTOPILOT";
    throw std::out_of_range(oss.str());
  }

  return 
      
       GetMoveMediator().ValidateDestination() &&
       GetMoveMediator().GetChosenDest().pnumbers.size() < 2
      
    ;
}

bool MerchantOfVenusGameInfo::AUTOPILOT_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::ProcessPilotNumber
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "AUTOPILOT";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to AUTOPILOT");
    return false;
  }

  
      
        if (GetMoveMediator().GetChosenDest().pnumbers.size() == 1)
        {
          GetMoveMediator().SetPilotNumber(*(GetMoveMediator().GetChosenDest().pnumbers.begin()));
        }
        return true;
      
    ;
}

bool MerchantOfVenusGameInfo::NOMULLIGAN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::Mulligan
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "NOMULLIGAN";
    throw std::out_of_range(oss.str());
  }

  return 
      
      i_PlayerName == GetPlayers().GetCurPlayer().GetName() && 
      GetMoveMediator().CanMulligan()
      
    ;
}

bool MerchantOfVenusGameInfo::NOMULLIGAN_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::Mulligan
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "NOMULLIGAN";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::NOMULLIGAN_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::Mulligan
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "NOMULLIGAN";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to NOMULLIGAN");
    return false;
  }

  
      
        MoveMediator& mm = GetMoveMediator();
        mm.ClearMulligan();
        return true;
      
    ;
}

bool MerchantOfVenusGameInfo::CANTMULLIGAN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::Mulligan
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "CANTMULLIGAN";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::CANTMULLIGAN_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::Mulligan
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "CANTMULLIGAN";
    throw std::out_of_range(oss.str());
  }

  return !GetMoveMediator().CanMulligan();
}

bool MerchantOfVenusGameInfo::CANTMULLIGAN_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::Mulligan
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "CANTMULLIGAN";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to CANTMULLIGAN");
    return false;
  }

  return true;;
}

bool MerchantOfVenusGameInfo::DONTLIKEOPTIONS_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::ValidateSetup
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

bool MerchantOfVenusGameInfo::DONTLIKEOPTIONS_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::ValidateSetup
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "DONTLIKEOPTIONS";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::DONTLIKEOPTIONS_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::ValidateSetup
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

bool MerchantOfVenusGameInfo::GAMENOTOVER_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TurnEnd
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GAMENOTOVER";
    throw std::out_of_range(oss.str());
  }

  return true;
}

bool MerchantOfVenusGameInfo::GAMENOTOVER_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TurnEnd
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GAMENOTOVER";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().GetCurPlayer().GetNetWorth() < m_winmoney
      
    ;
}

bool MerchantOfVenusGameInfo::GAMENOTOVER_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::TurnEnd
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GAMENOTOVER";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to GAMENOTOVER");
    return false;
  }

  
      GetPlayers().NextPlayer();
      BroadcastCURPLAYER();
      return true;
    ;
}

bool MerchantOfVenusGameInfo::SELL_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "SELL";
    throw std::out_of_range(oss.str());
  }

  return 
      
        i_PlayerName == GetPlayers().GetCurPlayer().GetName() 
      
    ;
}

bool MerchantOfVenusGameInfo::SELL_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "SELL";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::SELL_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "SELL";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 1)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to SELL");
    return false;
  }

  std::string i_item(boost::lexical_cast<std::string>(i_ap[0]));

  
      
      Player& curp = GetPlayers().GetCurPlayer();
      const std::pair<MapSpace,Secret> curpair = GetMapOverlay().GetMapSpace(curp.GetLocation());
      TradeBase& tb = GetTradeBases().GetTradeBaseBySolarSystemName(curpair.first.m_regionname);

      // validate the requested item
      std::vector<Token>::const_iterator tit;
      size_t tbidx = 0;
      for (tit = curp.GetTokens().begin() ; tit != curp.GetTokens().end() ; ++tit,++tbidx)
      {
        if (i_item == tit->Encoding()) break;
      }
      ERROUT(tit == curp.GetTokens().end(),"You don't own that item!");

      Token sellitem = *tit;

      ERROUT(!HasRemainingSell(),"You've already used your sell for this turn");
      ERROUT(sellitem.GetSizeClass() == SHIP_SIZE,"Ships cannot be sold");
      ERROUT(sellitem.GetSizeClass() == DEED_SIZE ||
             sellitem.GetSizeClass() == IOU_SIZE, "You can't sell paperwork");

      std::vector<std::string>::const_iterator bit;
      for (bit = sellitem.GetBuyers().begin() ; bit != sellitem.GetBuyers().end() ; ++bit)
      {
        if (tb.GetId() == *bit) break;
      }
  
      ERROUT(bit == sellitem.GetBuyers().end(),"The race here will not buy this item!");

      curp.RemoveToken(tbidx);
      BroadcastSUBTRACTFROMPLAYER(curp.GetId(),sellitem.Encoding());

      int finalsaleprice = ProcessDemands(*this,tb,GetTradeBases(),GetCup(),sellitem);

      switch(sellitem.GetSellType())
      {
        case CASH:
          curp.AddMoney(finalsaleprice);
          break;
        case BARTER:
          curp.AddBarter(finalsaleprice);
          break;
        default:
          break;
      }
      BroadcastUpdatePLAYERMONEYByplayerindex(curp.GetId());
      ApplyStationCommission(*this,GetPlayers(),GetMapOverlay(),finalsaleprice);
      DisposeOfToken(*this,GetTradeBases(),tb,GetCup(),sellitem);

      curp.IncrementSellCount();
      SENDTRANSCOUNT("");

      return true;
            
    ;
}

bool MerchantOfVenusGameInfo::MULLIGAN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::Mulligan
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "MULLIGAN";
    throw std::out_of_range(oss.str());
  }

  return 
      
      i_PlayerName == GetPlayers().GetCurPlayer().GetName() && 
      GetMoveMediator().CanMulligan()
      
    ;
}

bool MerchantOfVenusGameInfo::MULLIGAN_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::Mulligan
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "MULLIGAN";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::MULLIGAN_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::Mulligan
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "MULLIGAN";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 1)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to MULLIGAN");
    return false;
  }

  int i_numtoreroll(boost::lexical_cast<int>(i_ap[0]));

  
      
        MoveMediator& mm = GetMoveMediator();
        // the value given is the value on the die they wish to re-roll
        bool mresult = GetMoveMediator().DoMulligan(i_numtoreroll);
        ERROUT(!mresult,"That is not a valid die to reroll");
        BroadcastDICE(mm.GetDiceString());
        mm.PrepareForStep();
        mm.ClearMulligan();
        return true;
      
    ;
}

bool MerchantOfVenusGameInfo::EXECUTEMOVE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::FinalizeMove
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "EXECUTEMOVE";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::EXECUTEMOVE_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::FinalizeMove
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "EXECUTEMOVE";
    throw std::out_of_range(oss.str());
  }

  return true;
}

bool MerchantOfVenusGameInfo::EXECUTEMOVE_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::FinalizeMove
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "EXECUTEMOVE";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to EXECUTEMOVE");
    return false;
  }

  
      
        MoveMediator& mm = GetMoveMediator();
        MapOverlay& mo = GetMapOverlay();
        Player& cp = GetPlayers().GetCurPlayer();
        // moving from a penalty space (except for the first move)
        // costs money.
        if (!mm.IsFirst() && mm.GetPenaltyCost() > 0)
        {
          cp.AddMoney(-mm.GetPenaltyCost());
          BroadcastUpdatePLAYERMONEYByplayerindex(cp.GetId());
        }
        // we're no longer first.
        mm.ClearFirst();
 
        // any space a player lands on must become public if it isn't already.
        // furthermore, if the player moved blind into an empty space, 
        // the movement cost might change (drive move onto a colored penalty)
        // so repreparation once dest is public will be helpful.
        std::string dname = mm.GetChosenDest().sname;
        std::pair<MapSpace,Secret> destpair = mo.GetMapSpace(dname);
        if (!destpair.second.IsPublic())
        {
          mo.MakeSpacePublic(dname);
          destpair = mo.GetMapSpace(dname);
          BroadcastQBOXPRIVACY(dname,destpair.second.GetSecretString());
          BroadcastQBOXCONTENTS(dname,mo.GetSpaceString(dname));
          mm.PrepareForStep();
        }

        const Dest& dest = mm.GetChosenDest();

        // prevent backtracking.
        if (!dest.isjump)
        {
          mm.AddToBacks(cp.GetLocation(),dest.sname);
        }
        
        // decrement movement points
        mm.SpendMP(dest.mpused);
        BroadcastMPLEFT(mm.GetMP());
        BroadcastPILOTNUMBERCHOSEN(cp.GetId(),
                                   mm.GetPilotNumber(),
                                   mm.IsPilotNumberPermanent());

        // finally, make the move!
        cp.SetLocation(dest.sname);
        BroadcastUpdatePLAYERLOCATIONByplayerindex(cp.GetId()); 

        // now that we're in this new space, we need to take a look around.
        // if we are in an inhabited solar system that is not already known,
        // and we are in an observation spot (an orbit, or any spot if we have Spy Eye)
        // then we get to know who the solar system contains.
        if (GetTradeBases().GetBaseMap().find(destpair.first.m_regionname) != 
            GetTradeBases().GetBaseMap().end())
        {
          TradeBase& tb = GetTradeBases().GetTradeBaseBySolarSystemName(destpair.first.m_regionname);
          Secret& tbs = tb.GetSecret();
          if (!tbs.IsPublic()) 
          {
            if (destpair.first.m_orbit != "" ||
                cp.GetAdvances().HasSpyEye())
            {
              tbs.PlayerLearnsSecret(cp.GetId());
              BroadcastSOLARSYSTEMPRIVACY(destpair.first.m_regionname,tbs.GetSecretString());
              UnicastBASELOCATION(cp.GetName(),destpair.first.m_regionname,tb.GetId());
            }
          }
        }
        // further looking around.  if we have spy eye, then we get to know
        // about all adjacent qboxes we don't already know about.
        if (cp.GetAdvances().HasSpyEye())
        {
          std::vector<std::pair<std::string,bool> >::const_iterator adjit;
          for (adjit = destpair.first.m_adjacentnames.begin() ; 
               adjit != destpair.first.m_adjacentnames.end() ; ++adjit)
          {
            std::pair<MapSpace,Secret> adjpair = mo.GetMapSpace(adjit->first);
            if (adjpair.second.IsPublic()) continue;
            mo.AddPlayerToSecret(adjit->first,cp.GetId());
            adjpair = mo.GetMapSpace(adjit->first);
            BroadcastQBOXPRIVACY(adjit->first,adjpair.second.GetSecretString());
            UnicastQBOXCONTENTS(cp.GetName(),adjit->first,mo.GetSpaceString(adjit->first));
          }
        }

        mm.PrepareForStep();
        UnicastDESTINATION(cp.GetName(),
                           mm.GetPenaltyCost(),
                           mm.IsFirst(),
                           mm.GetCurrentDests());

        return true;
      
    ;
}

bool MerchantOfVenusGameInfo::VALIDATESETUP_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::InitialState
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

bool MerchantOfVenusGameInfo::VALIDATESETUP_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "VALIDATESETUP";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::VALIDATESETUP_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::InitialState
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

  
      
      for (size_t i = 0 ; i < GetPlayers().size() ; ++i)
      {
        GetPlayers()[i].SetOkStartup(false);
      }
      BroadcastPLAYEROKSTARTUP();
      return true;
      
    ;
}

bool MerchantOfVenusGameInfo::LEAVERELIC_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::AsteroidPhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "LEAVERELIC";
    throw std::out_of_range(oss.str());
  }

  return 
      
        i_PlayerName == GetPlayers().GetCurPlayer().GetName()
      
    ;
}

bool MerchantOfVenusGameInfo::LEAVERELIC_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::AsteroidPhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "LEAVERELIC";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::LEAVERELIC_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::AsteroidPhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "LEAVERELIC";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to LEAVERELIC");
    return false;
  }

  return true;;
}

bool MerchantOfVenusGameInfo::UNJOIN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::InitialState
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

bool MerchantOfVenusGameInfo::UNJOIN_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "UNJOIN";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::UNJOIN_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::InitialState
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
      for (size_t i = 0 ; i < GetPlayers().size() ; ++i)
      {
        UnicastIAM(GetPlayers()[i].GetName(),i);
      }
      BroadcastOPTIONS();


      return true;
      
    ;
}

bool MerchantOfVenusGameInfo::JETTISON_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "JETTISON";
    throw std::out_of_range(oss.str());
  }

  return 
      
        i_PlayerName == GetPlayers().GetCurPlayer().GetName()
      
    ;
}

bool MerchantOfVenusGameInfo::JETTISON_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "JETTISON";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::JETTISON_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "JETTISON";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 1)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to JETTISON");
    return false;
  }

  std::string i_item(boost::lexical_cast<std::string>(i_ap[0]));

  
      
      Player& curp = GetPlayers().GetCurPlayer();
      const std::pair<MapSpace,Secret> curpair = GetMapOverlay().GetMapSpace(curp.GetLocation());
      TradeBase& tb = GetTradeBases().GetTradeBaseBySolarSystemName(curpair.first.m_regionname);

      // validate the requested item
      std::vector<Token>::const_iterator tit;
      size_t tbidx = 0;
      for (tit = curp.GetTokens().begin() ; tit != curp.GetTokens().end() ; ++tit,++tbidx)
      {
        if (i_item == tit->Encoding())
        {
          break;
        }
      }
      ERROUT(tit != curp.GetTokens().end() && tit->IsNew(),"You can't jettison newly-bought items");
      ERROUT(tit == curp.GetTokens().end(),"You don't own that item!");

      Token sellitem = *tit;

      ERROUT(sellitem.GetSizeClass() == SHIP_SIZE,"Ships cannot be jettisoned");
      ERROUT(sellitem.GetSizeClass() == RELIC_SIZE,"You don't want to jettison a relic!");
      ERROUT(sellitem.GetSizeClass() == DEED_SIZE ||
             sellitem.GetSizeClass() == IOU_SIZE, "You can't jettison paperwork");

      curp.RemoveToken(tbidx);
      BroadcastSUBTRACTFROMPLAYER(curp.GetId(),sellitem.Encoding());

      DisposeOfToken(*this,GetTradeBases(),tb,GetCup(),sellitem);

      return true;
            
    ;
}

bool MerchantOfVenusGameInfo::GOTOTECHSELECT_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::StartTurn
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GOTOTECHSELECT";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::GOTOTECHSELECT_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::StartTurn
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GOTOTECHSELECT";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().GetCurPlayer().GetAdvances().GetSwitchables().size() > 0
      
    ;
}

bool MerchantOfVenusGameInfo::GOTOTECHSELECT_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::StartTurn
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GOTOTECHSELECT";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to GOTOTECHSELECT");
    return false;
  }

  
      UnicastSWITCHABLES(GetPlayers().GetCurPlayer().GetName(),
              GetPlayers().GetCurPlayer().GetAdvances().GetSwitchables());
      return true;
    ;
}

bool MerchantOfVenusGameInfo::ENDTURN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ENDTURN";
    throw std::out_of_range(oss.str());
  }

  return i_PlayerName == GetPlayers().GetCurPlayer().GetName();
}

bool MerchantOfVenusGameInfo::ENDTURN_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ENDTURN";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::ENDTURN_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ENDTURN";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to ENDTURN");
    return false;
  }

  
      
        Player& curp = GetPlayers().GetCurPlayer();
        curp.AgeTokens();
        curp.SetBarter(0);
        BroadcastUpdatePLAYERMONEYByplayerindex(curp.GetId());
        BroadcastAGEPLAYER(curp.GetId());
        return true;
      
    ;
}

bool MerchantOfVenusGameInfo::STARTGAME_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::ValidateSetup
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STARTGAME";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::STARTGAME_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::ValidateSetup
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "STARTGAME";
    throw std::out_of_range(oss.str());
  }

  return GetPlayers().AllOk();
}

bool MerchantOfVenusGameInfo::STARTGAME_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::ValidateSetup
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

  
      
        GetPlayers().RandomizeTurnOrder();
        BroadcastTURNORDER();
        BroadcastCURPLAYER();

        std::vector<std::string> raceids;
        raceids.push_back("3"); raceids.push_back("4a"); raceids.push_back("4b");
        raceids.push_back("7b"); raceids.push_back("8"); raceids.push_back("10");
        myshuffle(raceids.begin(),raceids.end());

        Token firstship(Ship::GetShipOfClass(SCOUT));

        for (size_t i = 0 ; i < GetPlayers().size() ; ++i)
        {
          GetPlayers()[i].AddToken(firstship);
          BroadcastADDTOPLAYER(i,firstship.Encoding());
          GetPlayers()[i].SetMoney(20 * GetPlayers().size());
          GetPlayers()[i].SetRace(raceids[i]);
          // player pawns are all at Galactic Base (we don't have to set this, but we do have to report it)
        }
        BroadcastPLAYERMONEY();
        BroadcastPLAYERRACE();
        BroadcastPLAYERLOCATION();

        // the following are all done via constructors
        //   provide hidden random to each qbox
        //   set up all race sets (off board to start with)
        //   set up Galactic Base stuff set (on board)
        //   provide hidden IOU to each inhabitable region
        // (we do, however, have to communicate the following:
        //   a) the contents of all trade bases (but not their location)
        //   b) the location of the 'base' trade base is public.
        CastAllTradeBases(*this,"",GetTradeBases());

        // make the galactic base trade base public.
        Secret& gbsecret = GetTradeBases().GetTradeBaseBySolarSystemName("Galactic Base").GetSecret();
        gbsecret.MakePublic();
        BroadcastSOLARSYSTEMPRIVACY("Galactic Base",gbsecret.GetSecretString());
        BroadcastBASELOCATION("Galactic Base","base");


        return true;
      
    ;
}

bool MerchantOfVenusGameInfo::ISRELIC_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::AutoTradePhaseSelect
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ISRELIC";
    throw std::out_of_range(oss.str());
  }

  return true;
}

bool MerchantOfVenusGameInfo::ISRELIC_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::AutoTradePhaseSelect
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ISRELIC";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetMapOverlay().HasRelic(GetPlayers().GetCurPlayer().GetLocation())
      
    ;
}

bool MerchantOfVenusGameInfo::ISRELIC_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::AutoTradePhaseSelect
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ISRELIC";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to ISRELIC");
    return false;
  }

  return true;;
}

bool MerchantOfVenusGameInfo::GOTOSELECTPILOTNUMBER_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::ProcessPilotNumber
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GOTOSELECTPILOTNUMBER";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::GOTOSELECTPILOTNUMBER_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::ProcessPilotNumber
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GOTOSELECTPILOTNUMBER";
    throw std::out_of_range(oss.str());
  }

  return 
      
       GetMoveMediator().ValidateDestination() &&
       GetMoveMediator().GetChosenDest().pnumbers.size() >= 2
      
    ;
}

bool MerchantOfVenusGameInfo::GOTOSELECTPILOTNUMBER_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::ProcessPilotNumber
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GOTOSELECTPILOTNUMBER";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to GOTOSELECTPILOTNUMBER");
    return false;
  }

  
      
        UnicastPILOTNUMBERCHOICE(GetPlayers().GetCurPlayer().GetName(),
                                 GetMoveMediator().GetChosenDest().GetPilotNumberString());
        return true;
      
    ;
}

bool MerchantOfVenusGameInfo::INSPACE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::AutoTradePhaseSelect
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "INSPACE";
    throw std::out_of_range(oss.str());
  }

  return true;
}

bool MerchantOfVenusGameInfo::INSPACE_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::AutoTradePhaseSelect
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "INSPACE";
    throw std::out_of_range(oss.str());
  }

  return 
      
      GetMapOverlay().GetMapSpace(GetPlayers().GetCurPlayer().GetLocation()).first.m_type != CITY &&
      GetMapOverlay().GetMapSpace(GetPlayers().GetCurPlayer().GetLocation()).first.m_type != STATION &&
      !GetMapOverlay().HasRelic(GetPlayers().GetCurPlayer().GetLocation())
      
    ;
}

bool MerchantOfVenusGameInfo::INSPACE_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::AutoTradePhaseSelect
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "INSPACE";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to INSPACE");
    return false;
  }

  return true;;
}

bool MerchantOfVenusGameInfo::SELECTSWITCHABLES_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TechSelect
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "SELECTSWITCHABLES";
    throw std::out_of_range(oss.str());
  }

  return i_PlayerName == GetPlayers().GetCurPlayer().GetName();
}

bool MerchantOfVenusGameInfo::SELECTSWITCHABLES_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TechSelect
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "SELECTSWITCHABLES";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::SELECTSWITCHABLES_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::TechSelect
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "SELECTSWITCHABLES";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 1)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to SELECTSWITCHABLES");
    return false;
  }

  std::string i_switchables(boost::lexical_cast<std::string>(i_ap[0]));

  
      bool sresult = GetPlayers().GetCurPlayer().GetAdvances().SetSwitchables(i_switchables);
      ERROUT(!sresult,"Invalid string for switchable equipment");
      BroadcastSELECTEDSWITCHABLES(GetPlayers().GetCurPlayerId(),i_switchables);
      return true;
    ;
}

bool MerchantOfVenusGameInfo::ISTRADEBASE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::AutoTradePhaseSelect
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ISTRADEBASE";
    throw std::out_of_range(oss.str());
  }

  return true;
}

bool MerchantOfVenusGameInfo::ISTRADEBASE_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::AutoTradePhaseSelect
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ISTRADEBASE";
    throw std::out_of_range(oss.str());
  }

  return 
      
      GetMapOverlay().GetMapSpace(GetPlayers().GetCurPlayer().GetLocation()).first.m_type == CITY ||
      GetMapOverlay().GetMapSpace(GetPlayers().GetCurPlayer().GetLocation()).first.m_type == STATION
      
    ;
}

bool MerchantOfVenusGameInfo::ISTRADEBASE_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::AutoTradePhaseSelect
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ISTRADEBASE";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to ISTRADEBASE");
    return false;
  }

  
      SENDTRANSCOUNT("");
      return true;
    ;
}

bool MerchantOfVenusGameInfo::SKIPTECHSELECT_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::StartTurn
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "SKIPTECHSELECT";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::SKIPTECHSELECT_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::StartTurn
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "SKIPTECHSELECT";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().GetCurPlayer().GetAdvances().GetSwitchables().size() == 0
      
    ;
}

bool MerchantOfVenusGameInfo::SKIPTECHSELECT_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::StartTurn
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "SKIPTECHSELECT";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to SKIPTECHSELECT");
    return false;
  }

  
      
      GetPlayers().GetCurPlayer().GetAdvances().SetSwitchables("");
      return true;
      
    ;
}

bool MerchantOfVenusGameInfo::ENDMOVE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::SelectMove
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ENDMOVE";
    throw std::out_of_range(oss.str());
  }

  return 
      
        i_PlayerName == GetPlayers().GetCurPlayer().GetName() && 
        GetMoveMediator().ManualStop()
      
    ;
}

bool MerchantOfVenusGameInfo::ENDMOVE_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::SelectMove
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ENDMOVE";
    throw std::out_of_range(oss.str());
  }

  return GetMoveMediator().AutoStop();
}

bool MerchantOfVenusGameInfo::ENDMOVE_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::SelectMove
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "ENDMOVE";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to ENDMOVE");
    return false;
  }

  
      
        Player& curp = GetPlayers().GetCurPlayer();
        if (GetMoveMediator().IsFirst() && GetMoveMediator().GetPenaltyCost() > 0)
        {
          curp.AddMoney(-GetMoveMediator().GetPenaltyCost());
          BroadcastUpdatePLAYERMONEYByplayerindex(curp.GetId());
        }

        std::pair<MapSpace,Secret> ms = GetMapOverlay().GetMapSpace(curp.GetLocation());
        

        // if player is standing on a CITY or STATION in a non-public Solar System,
        // a) make the solar system public.
        // b) find the IOU (if there is one) in the Solar System's Trade Base, remove it,
        //      and give it to the acting player.

        if (GetTradeBases().RegionHasTradeBase(ms.first.m_regionname) &&
            (ms.first.m_type == CITY || ms.first.m_type == STATION))
        {
          TradeBase& tb = GetTradeBases().GetTradeBaseBySolarSystemName(ms.first.m_regionname);
          if (!tb.GetSecret().IsPublic())
          {
            tb.GetSecret().MakePublic();
            BroadcastSOLARSYSTEMPRIVACY(ms.first.m_regionname,tb.GetSecret().GetSecretString());
            BroadcastBASELOCATION(ms.first.m_regionname,tb.GetId());

            for (size_t i = 0 ; i < tb.GetSpaceportDeeds().size() ; ++i)
            {
              BroadcastADDTOTRADEBASE(tb.GetId(),tb.GetSpaceportDeeds()[i].Encoding(),false);
            }
            tb.TransferDeeds();

            for (size_t i = 0 ; i < tb.GetSellableTokens().size() ; ++i)
            {
              if (tb.GetSellableTokens()[i].GetSizeClass() == IOU_SIZE)
              {
                Token iou = tb.GetSellableTokens()[i];
                curp.AddToken(iou);
                tb.RemoveSellableToken(i);
                BroadcastADDTOPLAYER(curp.GetId(),iou.Encoding());
                BroadcastSUBTRACTFROMTRADEBASE(tb.GetId(),iou.Encoding());
                break;
              }
            }
          }
        }
        return true;
      
    ;
}

bool MerchantOfVenusGameInfo::AUTOCHOOSEDEST_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::CompleteMove
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "AUTOCHOOSEDEST";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::AUTOCHOOSEDEST_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::CompleteMove
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "AUTOCHOOSEDEST";
    throw std::out_of_range(oss.str());
  }

  return 
      
        !GetMoveMediator().AutoStop() &&
        !GetMoveMediator().ManualStop() &&
        GetMoveMediator().GetDests().size() == 1
      
    ;
}

bool MerchantOfVenusGameInfo::AUTOCHOOSEDEST_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::CompleteMove
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "AUTOCHOOSEDEST";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to AUTOCHOOSEDEST");
    return false;
  }

  
      
        const Dest& theDest = GetMoveMediator().GetDests()[0];
        GetMoveMediator().SetDestination(theDest.sname,theDest.isjump);
        return true;      
      
    ;
}

bool MerchantOfVenusGameInfo::TRADEINSHIP_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "TRADEINSHIP";
    throw std::out_of_range(oss.str());
  }

  return 
      
        i_PlayerName == GetPlayers().GetCurPlayer().GetName()
      
    ;
}

bool MerchantOfVenusGameInfo::TRADEINSHIP_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "TRADEINSHIP";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::TRADEINSHIP_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "TRADEINSHIP";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 1)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to TRADEINSHIP");
    return false;
  }

  std::string i_newshipencoding(boost::lexical_cast<std::string>(i_ap[0]));

  
      
      Player& curp = GetPlayers().GetCurPlayer();
      const std::pair<MapSpace,Secret> curpair = GetMapOverlay().GetMapSpace(curp.GetLocation());
      TradeBase& tb = GetTradeBases().GetTradeBaseBySolarSystemName(curpair.first.m_regionname);

      // make sure that the requested encoding is something the trade base sells
      std::vector<Token>::const_iterator tit;
      for (tit = tb.GetSellableTokens().begin() ; tit != tb.GetSellableTokens().end() ; ++tit)
      {
        if (i_newshipencoding == tit->Encoding()) break;
      }
      ERROUT(tit == tb.GetSellableTokens().end(),"That item is not sold by this race");

      Token oldship = curp.GetShip();
      Token newship = *tit;
      ERROUT(newship.GetSizeClass() != SHIP_SIZE,"That item is not a SHIP!");

      int netcost = newship.GetBuyPrice() - oldship.GetSellPrice();
      ERROUT(netcost > curp.GetTotal(),"You can't afford that!");
      ERROUT(!HasRemainingBuy(),"You've already used your buy for this turn");
      ERROUT(!HasRemainingSell(),"You've already used your sell for this turn");

      ERROUT(newship.GetNumHolds() < curp.GetStuffSize(),"That new ship can't hold all your stuff!");

      BroadcastSUBTRACTFROMPLAYER(curp.GetId(),oldship.Encoding());
      BroadcastADDTOPLAYER(curp.GetId(),newship.Encoding());
      
      curp.AddBarter(oldship.GetSellPrice());
      curp.Spend(newship.GetBuyPrice());
      curp.ExchangeShip(newship);
      BroadcastUpdatePLAYERMONEYByplayerindex(curp.GetId());

      ApplyStationCommission(*this,GetPlayers(),GetMapOverlay(),newship.GetBuyPrice());
      ApplyStationCommission(*this,GetPlayers(),GetMapOverlay(),oldship.GetSellPrice());

      curp.IncrementBuyCount();
      curp.IncrementSellCount();
      SENDTRANSCOUNT("");
 
      return true;
      
    ;
}

bool MerchantOfVenusGameInfo::REJECTDESTINATION_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::ProcessPilotNumber
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "REJECTDESTINATION";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::REJECTDESTINATION_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::ProcessPilotNumber
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "REJECTDESTINATION";
    throw std::out_of_range(oss.str());
  }

  return !GetMoveMediator().ValidateDestination();
}

bool MerchantOfVenusGameInfo::REJECTDESTINATION_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::ProcessPilotNumber
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "REJECTDESTINATION";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to REJECTDESTINATION");
    return false;
  }

  
      
        UnicastMOVERROR(GetPlayers().GetCurPlayer().GetName(),"That is not a legal destination");
        return true;
      
    ;
}

bool MerchantOfVenusGameInfo::PICKUP_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "PICKUP";
    throw std::out_of_range(oss.str());
  }

  return 
      
        i_PlayerName == GetPlayers().GetCurPlayer().GetName()

      
    ;
}

bool MerchantOfVenusGameInfo::PICKUP_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "PICKUP";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::PICKUP_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "PICKUP";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 1)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to PICKUP");
    return false;
  }

  std::string i_item(boost::lexical_cast<std::string>(i_ap[0]));

  
      
      Player& curp = GetPlayers().GetCurPlayer();
      const std::pair<MapSpace,Secret> curpair = GetMapOverlay().GetMapSpace(curp.GetLocation());
      TradeBase& tb = GetTradeBases().GetTradeBaseBySolarSystemName(curpair.first.m_regionname);

      // make sure that the requested fare is something the trade base sells
      std::vector<Token>::const_iterator tit;
      size_t tbidx = 0;
      for (tit = tb.GetSellableTokens().begin() ; tit != tb.GetSellableTokens().end() ; ++tit,++tbidx)
      {
        if (i_item == tit->Encoding()) break;
      }
      ERROUT(tit == tb.GetSellableTokens().end(),"That item is not sold by this race");

      Token buyitem = *tit;
      ERROUT(buyitem.GetSizeClass() != FARE_SIZE,"Only Fares can be picked up");
      ERROUT(curp.GetShip().GetNumHolds() < curp.GetStuffSize(buyitem),"You can't fit that on your ship!");

      tb.RemoveSellableToken(tbidx);
      BroadcastSUBTRACTFROMTRADEBASE(tb.GetId(),buyitem.Encoding());

      buyitem.MakeNew();
      curp.AddToken(buyitem);
      BroadcastADDTOPLAYER(curp.GetId(),buyitem.Encoding());

      return true;
            
    ;
}

bool MerchantOfVenusGameInfo::SELECTDESTINATION_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::SelectMove
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "SELECTDESTINATION";
    throw std::out_of_range(oss.str());
  }

  return i_PlayerName == GetPlayers().GetCurPlayer().GetName();
}

bool MerchantOfVenusGameInfo::SELECTDESTINATION_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::SelectMove
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "SELECTDESTINATION";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::SELECTDESTINATION_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::SelectMove
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "SELECTDESTINATION";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 2)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to SELECTDESTINATION");
    return false;
  }

  std::string i_destination(boost::lexical_cast<std::string>(i_ap[0]));

  bool i_destisjump(boost::lexical_cast<bool>(i_ap[1]));

  
      
        MoveMediator& mm = GetMoveMediator();

        mm.SetDestination(i_destination,i_destisjump);

        if (!mm.AreDiceVisible())
        {
          // if we get here, the player's dest list (and choice) was created in absence
          // of pilot number.  Now that the player can see their dice, their choice
          // may be invalid.
          // however, if it isn't even theoretically valid, we shouldn't give them
          // their dice.
          ERROUT(!mm.ValidateDestination(),"Illegal First Move");

          mm.MakeDiceVisible();
          mm.PrepareForStep();

          UnicastDESTINATION(GetPlayers().GetCurPlayer().GetName(),
                             mm.GetPenaltyCost(),
                             mm.IsFirst(),
                             mm.GetCurrentDests());



          BroadcastDICE(mm.GetDiceString());
          BroadcastMPLEFT(mm.GetMP());
        }

        return true;
      
    ;
}

bool MerchantOfVenusGameInfo::LIKEOPTIONS_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::ValidateSetup
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

bool MerchantOfVenusGameInfo::LIKEOPTIONS_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::ValidateSetup
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "LIKEOPTIONS";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::LIKEOPTIONS_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::ValidateSetup
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

bool MerchantOfVenusGameInfo::OPTIONS_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "OPTIONS";
    throw std::out_of_range(oss.str());
  }

  return  GetPlayers().IsPlayer(i_PlayerName)  ;
}

bool MerchantOfVenusGameInfo::OPTIONS_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "OPTIONS";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::OPTIONS_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::InitialState
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "OPTIONS";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 1)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to OPTIONS");
    return false;
  }

  int winmoney(boost::lexical_cast<int>(i_ap[0]));

  if (winmoney < 1000)
  {
    UnicastERROR(i_PlayerName,"Action OPTIONS variable winmoney is below minimum");
    return false;
  }

  
      m_winmoney = winmoney;
      BroadcastOPTIONS();
      return true;
    ;
}

bool MerchantOfVenusGameInfo::MANUALCHOOSEDEST_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::CompleteMove
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "MANUALCHOOSEDEST";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::MANUALCHOOSEDEST_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::CompleteMove
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "MANUALCHOOSEDEST";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetMoveMediator().AutoStop() ||
        GetMoveMediator().ManualStop() ||
        GetMoveMediator().GetDests().size() != 1
      
    ;
}

bool MerchantOfVenusGameInfo::MANUALCHOOSEDEST_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::CompleteMove
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "MANUALCHOOSEDEST";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to MANUALCHOOSEDEST");
    return false;
  }

  return true;;
}

bool MerchantOfVenusGameInfo::SELECTPILOTNUMBER_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::SelectPilotNumber
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "SELECTPILOTNUMBER";
    throw std::out_of_range(oss.str());
  }

  return i_PlayerName == GetPlayers().GetCurPlayer().GetName();
}

bool MerchantOfVenusGameInfo::SELECTPILOTNUMBER_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::SelectPilotNumber
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "SELECTPILOTNUMBER";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::SELECTPILOTNUMBER_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::SelectPilotNumber
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "SELECTPILOTNUMBER";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 1)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to SELECTPILOTNUMBER");
    return false;
  }

  int i_pilotnumber(boost::lexical_cast<int>(i_ap[0]));

  
      
        ERROUT(GetMoveMediator().GetChosenDest().pnumbers.find(i_pilotnumber) == 
            GetMoveMediator().GetChosenDest().pnumbers.end(),"Illegal Pilot Number Chosen");
        GetMoveMediator().SetPilotNumber(i_pilotnumber);
        return true;
      
    ;
}

bool MerchantOfVenusGameInfo::REDEEMIOU_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "REDEEMIOU";
    throw std::out_of_range(oss.str());
  }

  return 
      
        i_PlayerName == GetPlayers().GetCurPlayer().GetName() &&
        GetIndexOfAppropriateIOU() != -1
      
    ;
}

bool MerchantOfVenusGameInfo::REDEEMIOU_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "REDEEMIOU";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::REDEEMIOU_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "REDEEMIOU";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to REDEEMIOU");
    return false;
  }

  
      
        Player& curp = GetPlayers().GetCurPlayer();
        int idx = GetIndexOfAppropriateIOU();
        Token t = curp.GetTokens()[(size_t)idx];
        curp.AddBarter(t.GetSellPrice());
        curp.RemoveToken((size_t)idx);
        BroadcastUpdatePLAYERMONEYByplayerindex(curp.GetId());
        BroadcastSUBTRACTFROMPLAYER(curp.GetId(),t.Encoding());

        ApplyStationCommission(*this,GetPlayers(),GetMapOverlay(),t.GetSellPrice());
        return true;
      
    ;
}

bool MerchantOfVenusGameInfo::BUY_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUY";
    throw std::out_of_range(oss.str());
  }

  return 
      
        i_PlayerName == GetPlayers().GetCurPlayer().GetName()
      
    ;
}

bool MerchantOfVenusGameInfo::BUY_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUY";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::BUY_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "BUY";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 1)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to BUY");
    return false;
  }

  std::string i_item(boost::lexical_cast<std::string>(i_ap[0]));

  
      
      Player& curp = GetPlayers().GetCurPlayer();
      const std::pair<MapSpace,Secret> curpair = GetMapOverlay().GetMapSpace(curp.GetLocation());
      TradeBase& tb = GetTradeBases().GetTradeBaseBySolarSystemName(curpair.first.m_regionname);

      // make sure that the requested encoding is something the trade base sells
      std::vector<Token>::const_iterator tit;
      size_t tbidx = 0;
      for (tit = tb.GetSellableTokens().begin() ; tit != tb.GetSellableTokens().end() ; ++tit,++tbidx)
      {
        if (i_item == tit->Encoding()) break;
      }
      ERROUT(tit == tb.GetSellableTokens().end(),"That item is not sold by this race");
  
      Token buyitem = *tit;

      ERROUT(!HasRemainingBuy(),"You've already used your buy for this turn");

      ERROUT(buyitem.GetSizeClass() == SHIP_SIZE,"you must trade in ships, not buy them");
      ERROUT(buyitem.GetSizeClass() == FARE_SIZE,"you must pick up fares, not buy them");
      ERROUT(buyitem.GetSizeClass() == IOU_SIZE ||
             buyitem.GetSizeClass() == DEMAND_SIZE,"you shouldn't be able to see one of these!?!");

      std::cout << "in Buy: num holds = " << curp.GetShip().GetNumHolds() <<
        " old stuffsize = " << curp.GetStuffSize() << 
        " new stuffsize = " << curp.GetStuffSize(buyitem) << std::endl;

      ERROUT(curp.GetShip().GetNumHolds() < curp.GetStuffSize(buyitem),"You can't fit that on your ship!");

      int buycost = buyitem.GetBuyPrice();
      if (buyitem.GetSizeClass() == DEED_SIZE &&
          curp.GetRace() == tb.GetId())
      {
        buycost -= buycost/5;
      }

      ERROUT(curp.GetTotal() < buycost,"You can't afford that!");

      if (!buyitem.IsUnlimited())
      {
        tb.RemoveSellableToken(tbidx);
        BroadcastSUBTRACTFROMTRADEBASE(tb.GetId(),buyitem.Encoding());
      }

      buyitem.MakeNew();

      curp.AddToken(buyitem);
      BroadcastADDTOPLAYER(curp.GetId(),buyitem.Encoding());
        
      curp.Spend(buycost);
      BroadcastUpdatePLAYERMONEYByplayerindex(curp.GetId());

      ApplyStationCommission(*this,GetPlayers(),GetMapOverlay(),buycost);

      if (buyitem.GetSizeClass() == DEED_SIZE)
      {
        switch(buyitem.GetDeedType())
        {
          case FACTORY_DEED:
            {
            GetMapOverlay().PlaceFactory(curpair.first.m_regionname,curp.GetId());
            BroadcastFACTORYOWNER(tb.GetId(),curp.GetId());
            Token facgood = tb.GetFactoryGood();
            tb.AddToken(facgood);
            BroadcastADDTOTRADEBASE(tb.GetId(),facgood.Encoding(),false);
            break;
            }
          case SPACEPORT_DEED:
            {
            std::string spacename = GetMapOverlay().PlaceStation(buyitem.GetName(),curp.GetId());
            BroadcastQBOXCONTENTS(spacename,GetMapOverlay().GetSpaceString(spacename));
            BroadcastQBOXPRIVACY(spacename,"public");
            break;
            }
          default:
            break;
        }
      }

      if (buyitem.GetSizeClass() == GOOD_SIZE && buyitem.IsFactoryGood())
      {
        ApplyFactoryCommission(*this,GetPlayers(),GetMapOverlay(),buyitem.GetBuyPrice());
      }

      curp.IncrementBuyCount();
      SENDTRANSCOUNT("");
 
      return true;
            
    ;
}

bool MerchantOfVenusGameInfo::DROPOFF_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "DROPOFF";
    throw std::out_of_range(oss.str());
  }

  return 
      
        i_PlayerName == GetPlayers().GetCurPlayer().GetName() 
      
    ;
}

bool MerchantOfVenusGameInfo::DROPOFF_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "DROPOFF";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::DROPOFF_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::TradePhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "DROPOFF";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 1)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to DROPOFF");
    return false;
  }

  std::string i_item(boost::lexical_cast<std::string>(i_ap[0]));

  
      
      Player& curp = GetPlayers().GetCurPlayer();
      const std::pair<MapSpace,Secret> curpair = GetMapOverlay().GetMapSpace(curp.GetLocation());
      TradeBase& tb = GetTradeBases().GetTradeBaseBySolarSystemName(curpair.first.m_regionname);

      // validate the requested fare.
      std::vector<Token>::const_iterator tit;
      size_t tbidx = 0;
      for (tit = curp.GetTokens().begin() ; tit != curp.GetTokens().end() ; ++tit,++tbidx)
      {
        if (i_item == tit->Encoding()) break;
      }
      ERROUT(tit == curp.GetTokens().end(),"You don't own that item!");

      Token sellitem = *tit;
      ERROUT(sellitem.GetSizeClass() != FARE_SIZE,"Only Fares can be dropped off");

      ERROUT(sellitem.GetBuyers()[0] != tb.GetId(),"This solar system is not this fare's destination!");

      curp.RemoveToken(tbidx);
      BroadcastSUBTRACTFROMPLAYER(curp.GetId(),sellitem.Encoding());

      curp.AddMoney(sellitem.GetSellPrice());
      BroadcastUpdatePLAYERMONEYByplayerindex(curp.GetId());

      DisposeOfToken(*this,GetTradeBases(),tb,GetCup(),sellitem);

      return true;
            
    ;
}

bool MerchantOfVenusGameInfo::GETRELIC_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::AsteroidPhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GETRELIC";
    throw std::out_of_range(oss.str());
  }

  return 
      
        i_PlayerName == GetPlayers().GetCurPlayer().GetName()
      
    ;
}

bool MerchantOfVenusGameInfo::GETRELIC_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::AsteroidPhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GETRELIC";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::GETRELIC_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::AsteroidPhase
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GETRELIC";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to GETRELIC");
    return false;
  }

  
      
        Player& curp = GetPlayers().GetCurPlayer();
        MapOverlay& mo = GetMapOverlay();
        Token relic = mo.TakeRelic(curp.GetLocation());
        curp.AddToken(relic);
        BroadcastADDTOPLAYER(curp.GetId(),relic.Encoding());
        BroadcastQBOXCONTENTS(curp.GetLocation(),mo.GetSpaceString(curp.GetLocation()));
        return true;
      
    ;
}

bool MerchantOfVenusGameInfo::GOTOCHOOSE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TechSelected
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GOTOCHOOSE";
    throw std::out_of_range(oss.str());
  }

  return false;
}

bool MerchantOfVenusGameInfo::GOTOCHOOSE_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TechSelected
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GOTOCHOOSE";
    throw std::out_of_range(oss.str());
  }

  return true;
}

bool MerchantOfVenusGameInfo::GOTOCHOOSE_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::TechSelected
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GOTOCHOOSE";
    throw std::out_of_range(oss.str());
  }

  if (i_ap.GetNumArguments() != 0)
  {
    UnicastERROR(i_PlayerName,"Bad number of arguments to GOTOCHOOSE");
    return false;
  }

  
      
      MoveMediator& mm = GetMoveMediator();
      Player& curp = GetPlayers().GetCurPlayer();
      curp.ResetExchangeCounts();
      mm.StartMove();
      BroadcastDICE(mm.GetDiceString());
      BroadcastPILOTNUMBERCHOSEN(curp.GetId(),0,false);

      mm.PrepareForStep();

      UnicastDESTINATION(curp.GetName(),
                         mm.GetPenaltyCost(),
                         mm.IsFirst(),
                         mm.GetCurrentDests());

      return true;
      
    ;
}

bool MerchantOfVenusGameInfo::GAMEOVER_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TurnEnd
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GAMEOVER";
    throw std::out_of_range(oss.str());
  }

  return true;
}

bool MerchantOfVenusGameInfo::GAMEOVER_IsAuto(const StateName &i_StateName) const
{
  if(i_StateName != MerchantOfVenusGameInfo::TurnEnd
  )
  {
    std::ostringstream oss;
    oss << "Unknown state " << i_StateName << " in transition " << "GAMEOVER";
    throw std::out_of_range(oss.str());
  }

  return 
      
        GetPlayers().GetCurPlayer().GetNetWorth() >= m_winmoney
      
    ;
}

bool MerchantOfVenusGameInfo::GAMEOVER_ExecuteAction(const std::string &i_PlayerName,
                                             const ActionParser &i_ap,
                                             const StateName &i_StateName)
{
  if(i_StateName != MerchantOfVenusGameInfo::TurnEnd
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

  
      BroadcastPLAYERWINS(GetPlayers().GetCurPlayer().GetId());
      return true;
    ;
}


void MerchantOfVenusGameInfo::SendFullState(const std::string &i_PlayerName,const StateName &i_StateName) const
{

    
      UnicastRESET(i_PlayerName);
      UnicastOPTIONS(i_PlayerName);
      UnicastPLAYER(i_PlayerName);

      if (GetPlayers().IsPlayer(i_PlayerName)) UnicastIAM(i_PlayerName,GetPlayers().GetPlayerId(i_PlayerName));

      if (i_StateName == InitialState) return;

      if (i_StateName == ValidateSetup)
      {
        UnicastPLAYEROKSTARTUP(i_PlayerName);
        return;
      }

      UnicastTURNORDER(i_PlayerName);
      UnicastCURPLAYER(i_PlayerName);

      UnicastPLAYERMONEY(i_PlayerName);
      UnicastPLAYERRACE(i_PlayerName);
      UnicastPLAYERLOCATION(i_PlayerName);
      SENDTRANSCOUNT(i_PlayerName);

      CastAllTradeBases(*this,i_PlayerName,GetTradeBases());
      UnicastTradeBaseSecrecy(*this,GetPlayers(),i_PlayerName,GetTradeBases());
      UnicastSpaceOverrides(*this,GetPlayers(),i_PlayerName,GetMapOverlay());

      for (size_t i = 0 ; i < GetPlayers().size() ; ++i)
      {
        const Player& pl = GetPlayers()[i];
        const std::vector<Token>& pltok = pl.GetTokens();
        for (size_t j = 0 ; j < pltok.size() ; ++j)
        {
          UnicastADDTOPLAYER(i_PlayerName,i,pltok[j].Encoding());
          if (pltok[j].GetSizeClass() == DEED_SIZE &&
              pltok[j].GetDeedType() == FACTORY_DEED)
          {
            UnicastFACTORYOWNER(i_PlayerName,pltok[j].GetSource(),pl.GetId());
          }
        }
      }

      if (i_StateName == TechSelect && i_PlayerName == GetPlayers().GetCurPlayer().GetName())
      {
        UnicastSWITCHABLES(i_PlayerName,
                GetPlayers().GetCurPlayer().GetAdvances().GetSwitchables());
      }
      else
      {
        UnicastSELECTEDSWITCHABLES(i_PlayerName,
                                   GetPlayers().GetCurPlayerId(),
                                   GetPlayers().GetCurPlayer().GetAdvances().GetActiveSwitchables());
      }

     
      if (i_StateName == SelectMove || i_StateName == ProcessPilotNumber || 
          i_StateName == SelectPilotNumber || i_StateName == Mulligan)
      {
        UnicastPILOTNUMBERCHOSEN(i_PlayerName,
                                 GetPlayers().GetCurPlayerId(),
                                 GetMoveMediator().GetPilotNumber(),
                                 GetMoveMediator().IsPilotNumberPermanent());

        if (i_PlayerName == GetPlayers().GetCurPlayer().GetName())
        {
          UnicastDESTINATION(i_PlayerName,
                             GetMoveMediator().GetPenaltyCost(),
                             GetMoveMediator().IsFirst(),
                             GetMoveMediator().GetCurrentDests());

          if (i_StateName == SelectPilotNumber)
          {
            UnicastPILOTNUMBERCHOICE(i_PlayerName,GetMoveMediator().GetChosenDest().GetPilotNumberString());
          }
        }


        UnicastDICE(i_PlayerName,GetMoveMediator().GetDiceString());

        if (GetMoveMediator().AreDiceVisible())
        {
          UnicastMPLEFT(i_PlayerName,GetMoveMediator().GetMP());
        }
      }

        
      if (i_StateName == TerminalState) 
        UnicastPLAYERWINS(i_PlayerName,GetPlayers().GetCurPlayer().GetId());


    
  
}

const StateName MerchantOfVenusGameInfo::CompleteMove = "CompleteMove";
const StateName MerchantOfVenusGameInfo::TurnEnd = "TurnEnd";
const StateName MerchantOfVenusGameInfo::TerminalState = "TerminalState";
const StateName MerchantOfVenusGameInfo::SelectPilotNumber = "SelectPilotNumber";
const StateName MerchantOfVenusGameInfo::InitialState = "InitialState";
const StateName MerchantOfVenusGameInfo::SelectMove = "SelectMove";
const StateName MerchantOfVenusGameInfo::AsteroidPhase = "AsteroidPhase";
const StateName MerchantOfVenusGameInfo::AutoTradePhaseSelect = "AutoTradePhaseSelect";
const StateName MerchantOfVenusGameInfo::TechSelected = "TechSelected";
const StateName MerchantOfVenusGameInfo::Mulligan = "Mulligan";
const StateName MerchantOfVenusGameInfo::StartTurn = "StartTurn";
const StateName MerchantOfVenusGameInfo::TechSelect = "TechSelect";
const StateName MerchantOfVenusGameInfo::TradePhase = "TradePhase";
const StateName MerchantOfVenusGameInfo::ValidateSetup = "ValidateSetup";
const StateName MerchantOfVenusGameInfo::ProcessPilotNumber = "ProcessPilotNumber";
const StateName MerchantOfVenusGameInfo::FinalizeMove = "FinalizeMove";
typedef DLLGame<MerchantOfVenusSet,MerchantOfVenusGameInfo> MerchantOfVenusGame;

bool JOIN_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.JOIN_IsLegal(i_PlayerName,i_StateName);
}

bool JOIN_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.JOIN_IsAuto(i_StateName);
}

bool JOIN_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.JOIN_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string JOIN_Transition::GetName() const
{
  return "JOIN";
}

bool AUTOPILOT_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.AUTOPILOT_IsLegal(i_PlayerName,i_StateName);
}

bool AUTOPILOT_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.AUTOPILOT_IsAuto(i_StateName);
}

bool AUTOPILOT_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.AUTOPILOT_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string AUTOPILOT_Transition::GetName() const
{
  return "AUTOPILOT";
}

bool NOMULLIGAN_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.NOMULLIGAN_IsLegal(i_PlayerName,i_StateName);
}

bool NOMULLIGAN_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.NOMULLIGAN_IsAuto(i_StateName);
}

bool NOMULLIGAN_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.NOMULLIGAN_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string NOMULLIGAN_Transition::GetName() const
{
  return "NOMULLIGAN";
}

bool CANTMULLIGAN_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.CANTMULLIGAN_IsLegal(i_PlayerName,i_StateName);
}

bool CANTMULLIGAN_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.CANTMULLIGAN_IsAuto(i_StateName);
}

bool CANTMULLIGAN_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.CANTMULLIGAN_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string CANTMULLIGAN_Transition::GetName() const
{
  return "CANTMULLIGAN";
}

bool DONTLIKEOPTIONS_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.DONTLIKEOPTIONS_IsLegal(i_PlayerName,i_StateName);
}

bool DONTLIKEOPTIONS_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.DONTLIKEOPTIONS_IsAuto(i_StateName);
}

bool DONTLIKEOPTIONS_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.DONTLIKEOPTIONS_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string DONTLIKEOPTIONS_Transition::GetName() const
{
  return "DONTLIKEOPTIONS";
}

bool GAMENOTOVER_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.GAMENOTOVER_IsLegal(i_PlayerName,i_StateName);
}

bool GAMENOTOVER_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.GAMENOTOVER_IsAuto(i_StateName);
}

bool GAMENOTOVER_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.GAMENOTOVER_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string GAMENOTOVER_Transition::GetName() const
{
  return "GAMENOTOVER";
}

bool SELL_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.SELL_IsLegal(i_PlayerName,i_StateName);
}

bool SELL_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.SELL_IsAuto(i_StateName);
}

bool SELL_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.SELL_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string SELL_Transition::GetName() const
{
  return "SELL";
}

bool MULLIGAN_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.MULLIGAN_IsLegal(i_PlayerName,i_StateName);
}

bool MULLIGAN_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.MULLIGAN_IsAuto(i_StateName);
}

bool MULLIGAN_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.MULLIGAN_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string MULLIGAN_Transition::GetName() const
{
  return "MULLIGAN";
}

bool EXECUTEMOVE_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.EXECUTEMOVE_IsLegal(i_PlayerName,i_StateName);
}

bool EXECUTEMOVE_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.EXECUTEMOVE_IsAuto(i_StateName);
}

bool EXECUTEMOVE_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.EXECUTEMOVE_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string EXECUTEMOVE_Transition::GetName() const
{
  return "EXECUTEMOVE";
}

bool VALIDATESETUP_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.VALIDATESETUP_IsLegal(i_PlayerName,i_StateName);
}

bool VALIDATESETUP_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.VALIDATESETUP_IsAuto(i_StateName);
}

bool VALIDATESETUP_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.VALIDATESETUP_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string VALIDATESETUP_Transition::GetName() const
{
  return "VALIDATESETUP";
}

bool LEAVERELIC_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.LEAVERELIC_IsLegal(i_PlayerName,i_StateName);
}

bool LEAVERELIC_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.LEAVERELIC_IsAuto(i_StateName);
}

bool LEAVERELIC_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.LEAVERELIC_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string LEAVERELIC_Transition::GetName() const
{
  return "LEAVERELIC";
}

bool UNJOIN_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.UNJOIN_IsLegal(i_PlayerName,i_StateName);
}

bool UNJOIN_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.UNJOIN_IsAuto(i_StateName);
}

bool UNJOIN_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.UNJOIN_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string UNJOIN_Transition::GetName() const
{
  return "UNJOIN";
}

bool JETTISON_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.JETTISON_IsLegal(i_PlayerName,i_StateName);
}

bool JETTISON_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.JETTISON_IsAuto(i_StateName);
}

bool JETTISON_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.JETTISON_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string JETTISON_Transition::GetName() const
{
  return "JETTISON";
}

bool GOTOTECHSELECT_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.GOTOTECHSELECT_IsLegal(i_PlayerName,i_StateName);
}

bool GOTOTECHSELECT_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.GOTOTECHSELECT_IsAuto(i_StateName);
}

bool GOTOTECHSELECT_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.GOTOTECHSELECT_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string GOTOTECHSELECT_Transition::GetName() const
{
  return "GOTOTECHSELECT";
}

bool ENDTURN_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.ENDTURN_IsLegal(i_PlayerName,i_StateName);
}

bool ENDTURN_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.ENDTURN_IsAuto(i_StateName);
}

bool ENDTURN_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.ENDTURN_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string ENDTURN_Transition::GetName() const
{
  return "ENDTURN";
}

bool STARTGAME_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.STARTGAME_IsLegal(i_PlayerName,i_StateName);
}

bool STARTGAME_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.STARTGAME_IsAuto(i_StateName);
}

bool STARTGAME_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.STARTGAME_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string STARTGAME_Transition::GetName() const
{
  return "STARTGAME";
}

bool ISRELIC_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.ISRELIC_IsLegal(i_PlayerName,i_StateName);
}

bool ISRELIC_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.ISRELIC_IsAuto(i_StateName);
}

bool ISRELIC_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.ISRELIC_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string ISRELIC_Transition::GetName() const
{
  return "ISRELIC";
}

bool GOTOSELECTPILOTNUMBER_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.GOTOSELECTPILOTNUMBER_IsLegal(i_PlayerName,i_StateName);
}

bool GOTOSELECTPILOTNUMBER_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.GOTOSELECTPILOTNUMBER_IsAuto(i_StateName);
}

bool GOTOSELECTPILOTNUMBER_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.GOTOSELECTPILOTNUMBER_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string GOTOSELECTPILOTNUMBER_Transition::GetName() const
{
  return "GOTOSELECTPILOTNUMBER";
}

bool INSPACE_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.INSPACE_IsLegal(i_PlayerName,i_StateName);
}

bool INSPACE_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.INSPACE_IsAuto(i_StateName);
}

bool INSPACE_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.INSPACE_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string INSPACE_Transition::GetName() const
{
  return "INSPACE";
}

bool SELECTSWITCHABLES_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.SELECTSWITCHABLES_IsLegal(i_PlayerName,i_StateName);
}

bool SELECTSWITCHABLES_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.SELECTSWITCHABLES_IsAuto(i_StateName);
}

bool SELECTSWITCHABLES_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.SELECTSWITCHABLES_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string SELECTSWITCHABLES_Transition::GetName() const
{
  return "SELECTSWITCHABLES";
}

bool ISTRADEBASE_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.ISTRADEBASE_IsLegal(i_PlayerName,i_StateName);
}

bool ISTRADEBASE_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.ISTRADEBASE_IsAuto(i_StateName);
}

bool ISTRADEBASE_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.ISTRADEBASE_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string ISTRADEBASE_Transition::GetName() const
{
  return "ISTRADEBASE";
}

bool SKIPTECHSELECT_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.SKIPTECHSELECT_IsLegal(i_PlayerName,i_StateName);
}

bool SKIPTECHSELECT_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.SKIPTECHSELECT_IsAuto(i_StateName);
}

bool SKIPTECHSELECT_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.SKIPTECHSELECT_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string SKIPTECHSELECT_Transition::GetName() const
{
  return "SKIPTECHSELECT";
}

bool ENDMOVE_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.ENDMOVE_IsLegal(i_PlayerName,i_StateName);
}

bool ENDMOVE_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.ENDMOVE_IsAuto(i_StateName);
}

bool ENDMOVE_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.ENDMOVE_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string ENDMOVE_Transition::GetName() const
{
  return "ENDMOVE";
}

bool AUTOCHOOSEDEST_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.AUTOCHOOSEDEST_IsLegal(i_PlayerName,i_StateName);
}

bool AUTOCHOOSEDEST_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.AUTOCHOOSEDEST_IsAuto(i_StateName);
}

bool AUTOCHOOSEDEST_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.AUTOCHOOSEDEST_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string AUTOCHOOSEDEST_Transition::GetName() const
{
  return "AUTOCHOOSEDEST";
}

bool TRADEINSHIP_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.TRADEINSHIP_IsLegal(i_PlayerName,i_StateName);
}

bool TRADEINSHIP_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.TRADEINSHIP_IsAuto(i_StateName);
}

bool TRADEINSHIP_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.TRADEINSHIP_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string TRADEINSHIP_Transition::GetName() const
{
  return "TRADEINSHIP";
}

bool REJECTDESTINATION_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.REJECTDESTINATION_IsLegal(i_PlayerName,i_StateName);
}

bool REJECTDESTINATION_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.REJECTDESTINATION_IsAuto(i_StateName);
}

bool REJECTDESTINATION_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.REJECTDESTINATION_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string REJECTDESTINATION_Transition::GetName() const
{
  return "REJECTDESTINATION";
}

bool PICKUP_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.PICKUP_IsLegal(i_PlayerName,i_StateName);
}

bool PICKUP_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.PICKUP_IsAuto(i_StateName);
}

bool PICKUP_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.PICKUP_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string PICKUP_Transition::GetName() const
{
  return "PICKUP";
}

bool SELECTDESTINATION_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.SELECTDESTINATION_IsLegal(i_PlayerName,i_StateName);
}

bool SELECTDESTINATION_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.SELECTDESTINATION_IsAuto(i_StateName);
}

bool SELECTDESTINATION_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.SELECTDESTINATION_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string SELECTDESTINATION_Transition::GetName() const
{
  return "SELECTDESTINATION";
}

bool LIKEOPTIONS_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.LIKEOPTIONS_IsLegal(i_PlayerName,i_StateName);
}

bool LIKEOPTIONS_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.LIKEOPTIONS_IsAuto(i_StateName);
}

bool LIKEOPTIONS_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.LIKEOPTIONS_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string LIKEOPTIONS_Transition::GetName() const
{
  return "LIKEOPTIONS";
}

bool OPTIONS_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.OPTIONS_IsLegal(i_PlayerName,i_StateName);
}

bool OPTIONS_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.OPTIONS_IsAuto(i_StateName);
}

bool OPTIONS_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.OPTIONS_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string OPTIONS_Transition::GetName() const
{
  return "OPTIONS";
}

bool MANUALCHOOSEDEST_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.MANUALCHOOSEDEST_IsLegal(i_PlayerName,i_StateName);
}

bool MANUALCHOOSEDEST_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.MANUALCHOOSEDEST_IsAuto(i_StateName);
}

bool MANUALCHOOSEDEST_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.MANUALCHOOSEDEST_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string MANUALCHOOSEDEST_Transition::GetName() const
{
  return "MANUALCHOOSEDEST";
}

bool SELECTPILOTNUMBER_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.SELECTPILOTNUMBER_IsLegal(i_PlayerName,i_StateName);
}

bool SELECTPILOTNUMBER_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.SELECTPILOTNUMBER_IsAuto(i_StateName);
}

bool SELECTPILOTNUMBER_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.SELECTPILOTNUMBER_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string SELECTPILOTNUMBER_Transition::GetName() const
{
  return "SELECTPILOTNUMBER";
}

bool REDEEMIOU_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.REDEEMIOU_IsLegal(i_PlayerName,i_StateName);
}

bool REDEEMIOU_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.REDEEMIOU_IsAuto(i_StateName);
}

bool REDEEMIOU_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.REDEEMIOU_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string REDEEMIOU_Transition::GetName() const
{
  return "REDEEMIOU";
}

bool BUY_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.BUY_IsLegal(i_PlayerName,i_StateName);
}

bool BUY_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.BUY_IsAuto(i_StateName);
}

bool BUY_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.BUY_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string BUY_Transition::GetName() const
{
  return "BUY";
}

bool DROPOFF_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.DROPOFF_IsLegal(i_PlayerName,i_StateName);
}

bool DROPOFF_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.DROPOFF_IsAuto(i_StateName);
}

bool DROPOFF_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.DROPOFF_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string DROPOFF_Transition::GetName() const
{
  return "DROPOFF";
}

bool GETRELIC_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.GETRELIC_IsLegal(i_PlayerName,i_StateName);
}

bool GETRELIC_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.GETRELIC_IsAuto(i_StateName);
}

bool GETRELIC_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.GETRELIC_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string GETRELIC_Transition::GetName() const
{
  return "GETRELIC";
}

bool GOTOCHOOSE_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.GOTOCHOOSE_IsLegal(i_PlayerName,i_StateName);
}

bool GOTOCHOOSE_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.GOTOCHOOSE_IsAuto(i_StateName);
}

bool GOTOCHOOSE_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.GOTOCHOOSE_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string GOTOCHOOSE_Transition::GetName() const
{
  return "GOTOCHOOSE";
}

bool GAMEOVER_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.GAMEOVER_IsLegal(i_PlayerName,i_StateName);
}

bool GAMEOVER_Transition::IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const
{
  return i_gi.GAMEOVER_IsAuto(i_StateName);
}

bool GAMEOVER_Transition::ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                           const std::string &i_PlayerName,
                           const ActionParser &i_ap) const
{
  return i_gi.GAMEOVER_ExecuteAction(i_PlayerName,i_ap,i_StateName);
}

std::string GAMEOVER_Transition::GetName() const
{
  return "GAMEOVER";
}

MerchantOfVenusStateMachine::MerchantOfVenusStateMachine()
{
  StateType *pCompleteMove = new StateType("CompleteMove","Auto-State for Move Completion");
  InsertState(pCompleteMove);
  StateType *pTurnEnd = new StateType("TurnEnd","Turn is Over");
  InsertState(pTurnEnd);
  StateType *pTerminalState = new StateType("TerminalState","End Of Game State");
  InsertState(pTerminalState);
  StateType *pSelectPilotNumber = new StateType("SelectPilotNumber","Select Which Pilot Number to use for a move");
  InsertState(pSelectPilotNumber);
  StateType *pInitialState = new StateType("InitialState","Initial Game State");
  InsertState(pInitialState);
  StateType *pSelectMove = new StateType("SelectMove","Select which adjacent space to move to");
  InsertState(pSelectMove);
  StateType *pAsteroidPhase = new StateType("AsteroidPhase","Special Trade Phase for Landing on an Asteroid");
  InsertState(pAsteroidPhase);
  StateType *pAutoTradePhaseSelect = new StateType("AutoTradePhaseSelect","Auto-State for Post-Move Determination");
  InsertState(pAutoTradePhaseSelect);
  StateType *pTechSelected = new StateType("TechSelected","Tech (if any) selected");
  InsertState(pTechSelected);
  StateType *pMulligan = new StateType("Mulligan","Decide whether to Mulligan");
  InsertState(pMulligan);
  StateType *pStartTurn = new StateType("StartTurn","Start of Turn");
  InsertState(pStartTurn);
  StateType *pTechSelect = new StateType("TechSelect","Select Which Tech to Turn On");
  InsertState(pTechSelect);
  StateType *pTradePhase = new StateType("TradePhase","Engage in trading");
  InsertState(pTradePhase);
  StateType *pValidateSetup = new StateType("ValidateSetup","Give all Players an Opportunity to ok Players and Options");
  InsertState(pValidateSetup);
  StateType *pProcessPilotNumber = new StateType("ProcessPilotNumber","Auto-State for Pilot Number Selection");
  InsertState(pProcessPilotNumber);
  StateType *pFinalizeMove = new StateType("FinalizeMove","Auto-State for Move Finalization");
  InsertState(pFinalizeMove);
  TransitionType *pJOIN_Transition = new JOIN_Transition;
  InsertTransition(pJOIN_Transition);
  TransitionType *pAUTOPILOT_Transition = new AUTOPILOT_Transition;
  InsertTransition(pAUTOPILOT_Transition);
  TransitionType *pNOMULLIGAN_Transition = new NOMULLIGAN_Transition;
  InsertTransition(pNOMULLIGAN_Transition);
  TransitionType *pCANTMULLIGAN_Transition = new CANTMULLIGAN_Transition;
  InsertTransition(pCANTMULLIGAN_Transition);
  TransitionType *pDONTLIKEOPTIONS_Transition = new DONTLIKEOPTIONS_Transition;
  InsertTransition(pDONTLIKEOPTIONS_Transition);
  TransitionType *pGAMENOTOVER_Transition = new GAMENOTOVER_Transition;
  InsertTransition(pGAMENOTOVER_Transition);
  TransitionType *pSELL_Transition = new SELL_Transition;
  InsertTransition(pSELL_Transition);
  TransitionType *pMULLIGAN_Transition = new MULLIGAN_Transition;
  InsertTransition(pMULLIGAN_Transition);
  TransitionType *pEXECUTEMOVE_Transition = new EXECUTEMOVE_Transition;
  InsertTransition(pEXECUTEMOVE_Transition);
  TransitionType *pVALIDATESETUP_Transition = new VALIDATESETUP_Transition;
  InsertTransition(pVALIDATESETUP_Transition);
  TransitionType *pLEAVERELIC_Transition = new LEAVERELIC_Transition;
  InsertTransition(pLEAVERELIC_Transition);
  TransitionType *pUNJOIN_Transition = new UNJOIN_Transition;
  InsertTransition(pUNJOIN_Transition);
  TransitionType *pJETTISON_Transition = new JETTISON_Transition;
  InsertTransition(pJETTISON_Transition);
  TransitionType *pGOTOTECHSELECT_Transition = new GOTOTECHSELECT_Transition;
  InsertTransition(pGOTOTECHSELECT_Transition);
  TransitionType *pENDTURN_Transition = new ENDTURN_Transition;
  InsertTransition(pENDTURN_Transition);
  TransitionType *pSTARTGAME_Transition = new STARTGAME_Transition;
  InsertTransition(pSTARTGAME_Transition);
  TransitionType *pISRELIC_Transition = new ISRELIC_Transition;
  InsertTransition(pISRELIC_Transition);
  TransitionType *pGOTOSELECTPILOTNUMBER_Transition = new GOTOSELECTPILOTNUMBER_Transition;
  InsertTransition(pGOTOSELECTPILOTNUMBER_Transition);
  TransitionType *pINSPACE_Transition = new INSPACE_Transition;
  InsertTransition(pINSPACE_Transition);
  TransitionType *pSELECTSWITCHABLES_Transition = new SELECTSWITCHABLES_Transition;
  InsertTransition(pSELECTSWITCHABLES_Transition);
  TransitionType *pISTRADEBASE_Transition = new ISTRADEBASE_Transition;
  InsertTransition(pISTRADEBASE_Transition);
  TransitionType *pSKIPTECHSELECT_Transition = new SKIPTECHSELECT_Transition;
  InsertTransition(pSKIPTECHSELECT_Transition);
  TransitionType *pENDMOVE_Transition = new ENDMOVE_Transition;
  InsertTransition(pENDMOVE_Transition);
  TransitionType *pAUTOCHOOSEDEST_Transition = new AUTOCHOOSEDEST_Transition;
  InsertTransition(pAUTOCHOOSEDEST_Transition);
  TransitionType *pTRADEINSHIP_Transition = new TRADEINSHIP_Transition;
  InsertTransition(pTRADEINSHIP_Transition);
  TransitionType *pREJECTDESTINATION_Transition = new REJECTDESTINATION_Transition;
  InsertTransition(pREJECTDESTINATION_Transition);
  TransitionType *pPICKUP_Transition = new PICKUP_Transition;
  InsertTransition(pPICKUP_Transition);
  TransitionType *pSELECTDESTINATION_Transition = new SELECTDESTINATION_Transition;
  InsertTransition(pSELECTDESTINATION_Transition);
  TransitionType *pLIKEOPTIONS_Transition = new LIKEOPTIONS_Transition;
  InsertTransition(pLIKEOPTIONS_Transition);
  TransitionType *pOPTIONS_Transition = new OPTIONS_Transition;
  InsertTransition(pOPTIONS_Transition);
  TransitionType *pMANUALCHOOSEDEST_Transition = new MANUALCHOOSEDEST_Transition;
  InsertTransition(pMANUALCHOOSEDEST_Transition);
  TransitionType *pSELECTPILOTNUMBER_Transition = new SELECTPILOTNUMBER_Transition;
  InsertTransition(pSELECTPILOTNUMBER_Transition);
  TransitionType *pREDEEMIOU_Transition = new REDEEMIOU_Transition;
  InsertTransition(pREDEEMIOU_Transition);
  TransitionType *pBUY_Transition = new BUY_Transition;
  InsertTransition(pBUY_Transition);
  TransitionType *pDROPOFF_Transition = new DROPOFF_Transition;
  InsertTransition(pDROPOFF_Transition);
  TransitionType *pGETRELIC_Transition = new GETRELIC_Transition;
  InsertTransition(pGETRELIC_Transition);
  TransitionType *pGOTOCHOOSE_Transition = new GOTOCHOOSE_Transition;
  InsertTransition(pGOTOCHOOSE_Transition);
  TransitionType *pGAMEOVER_Transition = new GAMEOVER_Transition;
  InsertTransition(pGAMEOVER_Transition);

  pInitialState->InsertTransition(*pJOIN_Transition,*pInitialState);
  pProcessPilotNumber->InsertTransition(*pAUTOPILOT_Transition,*pFinalizeMove);
  pMulligan->InsertTransition(*pNOMULLIGAN_Transition,*pProcessPilotNumber);
  pMulligan->InsertTransition(*pCANTMULLIGAN_Transition,*pProcessPilotNumber);
  pValidateSetup->InsertTransition(*pDONTLIKEOPTIONS_Transition,*pInitialState);
  pTurnEnd->InsertTransition(*pGAMENOTOVER_Transition,*pStartTurn);
  pTradePhase->InsertTransition(*pSELL_Transition,*pTradePhase);
  pMulligan->InsertTransition(*pMULLIGAN_Transition,*pProcessPilotNumber);
  pFinalizeMove->InsertTransition(*pEXECUTEMOVE_Transition,*pCompleteMove);
  pInitialState->InsertTransition(*pVALIDATESETUP_Transition,*pValidateSetup);
  pAsteroidPhase->InsertTransition(*pLEAVERELIC_Transition,*pTurnEnd);
  pInitialState->InsertTransition(*pUNJOIN_Transition,*pInitialState);
  pTradePhase->InsertTransition(*pJETTISON_Transition,*pTradePhase);
  pStartTurn->InsertTransition(*pGOTOTECHSELECT_Transition,*pTechSelect);
  pTradePhase->InsertTransition(*pENDTURN_Transition,*pTurnEnd);
  pValidateSetup->InsertTransition(*pSTARTGAME_Transition,*pStartTurn);
  pAutoTradePhaseSelect->InsertTransition(*pISRELIC_Transition,*pAsteroidPhase);
  pProcessPilotNumber->InsertTransition(*pGOTOSELECTPILOTNUMBER_Transition,*pSelectPilotNumber);
  pAutoTradePhaseSelect->InsertTransition(*pINSPACE_Transition,*pTurnEnd);
  pTechSelect->InsertTransition(*pSELECTSWITCHABLES_Transition,*pTechSelected);
  pAutoTradePhaseSelect->InsertTransition(*pISTRADEBASE_Transition,*pTradePhase);
  pStartTurn->InsertTransition(*pSKIPTECHSELECT_Transition,*pTechSelected);
  pSelectMove->InsertTransition(*pENDMOVE_Transition,*pAutoTradePhaseSelect);
  pCompleteMove->InsertTransition(*pAUTOCHOOSEDEST_Transition,*pProcessPilotNumber);
  pTradePhase->InsertTransition(*pTRADEINSHIP_Transition,*pTradePhase);
  pProcessPilotNumber->InsertTransition(*pREJECTDESTINATION_Transition,*pSelectMove);
  pTradePhase->InsertTransition(*pPICKUP_Transition,*pTradePhase);
  pSelectMove->InsertTransition(*pSELECTDESTINATION_Transition,*pMulligan);
  pValidateSetup->InsertTransition(*pLIKEOPTIONS_Transition,*pValidateSetup);
  pInitialState->InsertTransition(*pOPTIONS_Transition,*pInitialState);
  pCompleteMove->InsertTransition(*pMANUALCHOOSEDEST_Transition,*pSelectMove);
  pSelectPilotNumber->InsertTransition(*pSELECTPILOTNUMBER_Transition,*pFinalizeMove);
  pTradePhase->InsertTransition(*pREDEEMIOU_Transition,*pTradePhase);
  pTradePhase->InsertTransition(*pBUY_Transition,*pTradePhase);
  pTradePhase->InsertTransition(*pDROPOFF_Transition,*pTradePhase);
  pAsteroidPhase->InsertTransition(*pGETRELIC_Transition,*pTurnEnd);
  pTechSelected->InsertTransition(*pGOTOCHOOSE_Transition,*pSelectMove);
  pTurnEnd->InsertTransition(*pGAMEOVER_Transition,*pTerminalState);
}

MerchantOfVenusSet *pGameSet;
MerchantOfVenusStateMachine *pStateMachine;

bool Initialize(const std::string &i_DataDir)
{
  pStateMachine = new MerchantOfVenusStateMachine;
  pGameSet = new MerchantOfVenusSet(i_DataDir);
  if (!pGameSet || !pGameSet->IsValid())
  {
    return false;
  }
  return true;
}

Game *CreateGame(const ServerGameInfo &i_rServerGameInfo,OutputPort &i_rOutputPort)
{
  MerchantOfVenusGame *pNewGame = new MerchantOfVenusGame(i_rServerGameInfo,i_rOutputPort,
                                   *pGameSet,*pStateMachine,100);
  return pNewGame;
}
