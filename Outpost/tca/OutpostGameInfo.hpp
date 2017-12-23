#ifndef OutpostGAMEINFOHPP
#define OutpostGAMEINFOHPP
#include "GameInfo.hpp"
#include "OutpostSet.hpp"
#include "OutpostState.hpp"
#include "StateMachine.hpp"
#include "StringUtilities.hpp"

class NameBoolean;

class OutpostGameInfo : public GameInfo<OutpostSet,OutpostState>
{
public:
  OutpostGameInfo(const OutpostSet &i_Set,
                  const ServerGameInfo &i_ginfo,
                  OutputPort  &i_Port);
private:
  static const StateName Bid;
  static const StateName FinalManning;
  static const StateName MegaResources;
  static const StateName ManualDiscard;
  static const StateName TurnEnd;
  static const StateName TerminalState;
  static const StateName TurnOrder;
  static const StateName InitialState;
  static const StateName StartResources;
  static const StateName DoDiscard;
  static const StateName SupplyFill;
  static const StateName StartPurchase;
  static const StateName ValidateSetup;
  static const StateName PromptMegaResources;
  static const StateName Purchase;
  static const StateName StartDiscard;
  static const StateName StartFinalManning;
  mutable int m_errorcount;
public:
  void ResetERRORCount();
  int GetERRORCount() const;
  bool PASS_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool PASS_IsAuto(const StateName &i_StateName) const;
  bool PASS_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool FILLSHIP_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool FILLSHIP_IsAuto(const StateName &i_StateName) const;
  bool FILLSHIP_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool JOIN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool JOIN_IsAuto(const StateName &i_StateName) const;
  bool JOIN_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool OPENBID_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool OPENBID_IsAuto(const StateName &i_StateName) const;
  bool OPENBID_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool BUYOREFACTORIES_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool BUYOREFACTORIES_IsAuto(const StateName &i_StateName) const;
  bool BUYOREFACTORIES_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool DONTLIKEOPTIONS_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool DONTLIKEOPTIONS_IsAuto(const StateName &i_StateName) const;
  bool DONTLIKEOPTIONS_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool DISCARD_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool DISCARD_IsAuto(const StateName &i_StateName) const;
  bool DISCARD_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool PURCHASEBID_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool PURCHASEBID_IsAuto(const StateName &i_StateName) const;
  bool PURCHASEBID_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool AUTOSKIPDISCARD_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool AUTOSKIPDISCARD_IsAuto(const StateName &i_StateName) const;
  bool AUTOSKIPDISCARD_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool BUYTITANIUMFACTORIES_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool BUYTITANIUMFACTORIES_IsAuto(const StateName &i_StateName) const;
  bool BUYTITANIUMFACTORIES_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool DETERMINETURNORDER_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool DETERMINETURNORDER_IsAuto(const StateName &i_StateName) const;
  bool DETERMINETURNORDER_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool DONEMANNING_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool DONEMANNING_IsAuto(const StateName &i_StateName) const;
  bool DONEMANNING_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool VALIDATESETUP_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool VALIDATESETUP_IsAuto(const StateName &i_StateName) const;
  bool VALIDATESETUP_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool BUYWATERFACTORIES_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool BUYWATERFACTORIES_IsAuto(const StateName &i_StateName) const;
  bool BUYWATERFACTORIES_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool ENDDISCARDTOTURNEND_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool ENDDISCARDTOTURNEND_IsAuto(const StateName &i_StateName) const;
  bool ENDDISCARDTOTURNEND_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool COMMODITYDONETOPURCHASE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool COMMODITYDONETOPURCHASE_IsAuto(const StateName &i_StateName) const;
  bool COMMODITYDONETOPURCHASE_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool UNJOIN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool UNJOIN_IsAuto(const StateName &i_StateName) const;
  bool UNJOIN_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool DONE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool DONE_IsAuto(const StateName &i_StateName) const;
  bool DONE_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool NEWTURN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool NEWTURN_IsAuto(const StateName &i_StateName) const;
  bool NEWTURN_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool STARTGAME_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool STARTGAME_IsAuto(const StateName &i_StateName) const;
  bool STARTGAME_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool STANDARDBASIC_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool STANDARDBASIC_IsAuto(const StateName &i_StateName) const;
  bool STANDARDBASIC_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool BID_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool BID_IsAuto(const StateName &i_StateName) const;
  bool BID_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool STARTPURCHASE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool STARTPURCHASE_IsAuto(const StateName &i_StateName) const;
  bool STARTPURCHASE_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool COMMODITYDONETODISCARD_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool COMMODITYDONETODISCARD_IsAuto(const StateName &i_StateName) const;
  bool COMMODITYDONETODISCARD_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool NUMMEGAS_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool NUMMEGAS_IsAuto(const StateName &i_StateName) const;
  bool NUMMEGAS_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool STARTDISCARD_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool STARTDISCARD_IsAuto(const StateName &i_StateName) const;
  bool STARTDISCARD_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool MANUALDISCARD_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool MANUALDISCARD_IsAuto(const StateName &i_StateName) const;
  bool MANUALDISCARD_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool PASSOUT_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool PASSOUT_IsAuto(const StateName &i_StateName) const;
  bool PASSOUT_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool ENDMANNINGTOTURNEND_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool ENDMANNINGTOTURNEND_IsAuto(const StateName &i_StateName) const;
  bool ENDMANNINGTOTURNEND_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool LIKEOPTIONS_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool LIKEOPTIONS_IsAuto(const StateName &i_StateName) const;
  bool LIKEOPTIONS_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool OPTIONS_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool OPTIONS_IsAuto(const StateName &i_StateName) const;
  bool OPTIONS_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool ENDMANNINGTODISCARD_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool ENDMANNINGTODISCARD_IsAuto(const StateName &i_StateName) const;
  bool ENDMANNINGTODISCARD_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool BUYNEWCHEMFACTORIES_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool BUYNEWCHEMFACTORIES_IsAuto(const StateName &i_StateName) const;
  bool BUYNEWCHEMFACTORIES_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool STARTMANNING_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool STARTMANNING_IsAuto(const StateName &i_StateName) const;
  bool STARTMANNING_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool BUYROBOTS_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool BUYROBOTS_IsAuto(const StateName &i_StateName) const;
  bool BUYROBOTS_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool INTERNALIZE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool INTERNALIZE_IsAuto(const StateName &i_StateName) const;
  bool INTERNALIZE_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool ALTERMANNING_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool ALTERMANNING_IsAuto(const StateName &i_StateName) const;
  bool ALTERMANNING_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool ENDDISCARDTOPURCHASE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool ENDDISCARDTOPURCHASE_IsAuto(const StateName &i_StateName) const;
  bool ENDDISCARDTOPURCHASE_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool BUYRESEARCHFACTORIES_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool BUYRESEARCHFACTORIES_IsAuto(const StateName &i_StateName) const;
  bool BUYRESEARCHFACTORIES_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool FINALDONE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool FINALDONE_IsAuto(const StateName &i_StateName) const;
  bool FINALDONE_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool WATERMULLIGAN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool WATERMULLIGAN_IsAuto(const StateName &i_StateName) const;
  bool WATERMULLIGAN_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool INITCOMMODITY_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool INITCOMMODITY_IsAuto(const StateName &i_StateName) const;
  bool INITCOMMODITY_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool STANDARDEXPERT_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool STANDARDEXPERT_IsAuto(const StateName &i_StateName) const;
  bool STANDARDEXPERT_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool BUYMEN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool BUYMEN_IsAuto(const StateName &i_StateName) const;
  bool BUYMEN_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool PURCHASEDONE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool PURCHASEDONE_IsAuto(const StateName &i_StateName) const;
  bool PURCHASEDONE_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool GAMEOVER_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool GAMEOVER_IsAuto(const StateName &i_StateName) const;
  bool GAMEOVER_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool REQUESTMEGA_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool REQUESTMEGA_IsAuto(const StateName &i_StateName) const;
  bool REQUESTMEGA_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  void SendFullState(const std::string &i_PlayerName,const StateName &i_StateName) const;


  void UnicastCOMMODITYSTOCK(const std::string &i_PlayerName) const;
  void VaricastCOMMODITYSTOCK(const NameBoolean &i_Variator) const;
  void BroadcastCOMMODITYSTOCK() const;
  void UnicastUpdateCOMMODITYSTOCKBycommType(const std::string &i_PlayerName,
        const int& commType) const;
  void VaricastUpdateCOMMODITYSTOCKBycommType(const NameBoolean &i_Variator,
        const int& commType) const;
  void BroadcastUpdateCOMMODITYSTOCKBycommType(
        const int& commType) const;

  void UnicastPLAYERFACTORIES(const std::string &i_PlayerName) const;
  void VaricastPLAYERFACTORIES(const NameBoolean &i_Variator) const;
  void BroadcastPLAYERFACTORIES() const;
  void UnicastUpdatePLAYERFACTORIESByplayerindex(const std::string &i_PlayerName,
        const size_t& playerindex) const;
  void VaricastUpdatePLAYERFACTORIESByplayerindex(const NameBoolean &i_Variator,
        const size_t& playerindex) const;
  void BroadcastUpdatePLAYERFACTORIESByplayerindex(
        const size_t& playerindex) const;

  void UnicastPLAYER(const std::string &i_PlayerName) const;
  void VaricastPLAYER(const NameBoolean &i_Variator) const;
  void BroadcastPLAYER() const;
  void UnicastUpdatePLAYERByplayerindex(const std::string &i_PlayerName,
        const size_t& playerindex) const;
  void VaricastUpdatePLAYERByplayerindex(const NameBoolean &i_Variator,
        const size_t& playerindex) const;
  void BroadcastUpdatePLAYERByplayerindex(
        const size_t& playerindex) const;

  void UnicastPLAYERCARDBACKS(const std::string &i_PlayerName) const;
  void VaricastPLAYERCARDBACKS(const NameBoolean &i_Variator) const;
  void BroadcastPLAYERCARDBACKS() const;
  void UnicastUpdatePLAYERCARDBACKSByplayerindex(const std::string &i_PlayerName,
        const size_t& playerindex) const;
  void VaricastUpdatePLAYERCARDBACKSByplayerindex(const NameBoolean &i_Variator,
        const size_t& playerindex) const;
  void BroadcastUpdatePLAYERCARDBACKSByplayerindex(
        const size_t& playerindex) const;

  void UnicastSPENDS(const std::string &i_PlayerName) const;
  void VaricastSPENDS(const NameBoolean &i_Variator) const;
  void BroadcastSPENDS() const;
  void UnicastUpdateSPENDSByspendIndex(const std::string &i_PlayerName,
        const size_t& spendIndex) const;
  void VaricastUpdateSPENDSByspendIndex(const NameBoolean &i_Variator,
        const size_t& spendIndex) const;
  void BroadcastUpdateSPENDSByspendIndex(
        const size_t& spendIndex) const;

  void UnicastERROR(const std::string &i_PlayerName,const std::string& i_ErrorText) const;
  void VaricastERROR(const NameBoolean &i_Variator,const std::string& i_ErrorText) const;
  void BroadcastERROR(const std::string& i_ErrorText) const;

  void UnicastSUPPLYSHIP(const std::string &i_PlayerName) const;
  void VaricastSUPPLYSHIP(const NameBoolean &i_Variator) const;
  void BroadcastSUPPLYSHIP() const;
  void UnicastUpdateSUPPLYSHIPByshipIndex(const std::string &i_PlayerName,
        const size_t& shipIndex) const;
  void VaricastUpdateSUPPLYSHIPByshipIndex(const NameBoolean &i_Variator,
        const size_t& shipIndex) const;
  void BroadcastUpdateSUPPLYSHIPByshipIndex(
        const size_t& shipIndex) const;

  void UnicastPLAYERWINS(const std::string &i_PlayerName,const size_t& i_PId) const;
  void VaricastPLAYERWINS(const NameBoolean &i_Variator,const size_t& i_PId) const;
  void BroadcastPLAYERWINS(const size_t& i_PId) const;

  void UnicastOPTIONS(const std::string &i_PlayerName) const;
  void VaricastOPTIONS(const NameBoolean &i_Variator) const;
  void BroadcastOPTIONS() const;

  void UnicastBIDITEM(const std::string &i_PlayerName) const;
  void VaricastBIDITEM(const NameBoolean &i_Variator) const;
  void BroadcastBIDITEM() const;

  void SingleCastPLAYERPURCHASEINFO(const size_t& playerindex) const;
  void AllCastPLAYERPURCHASEINFO() const;

  void UnicastPLAYERSTATE(const std::string &i_PlayerName) const;
  void VaricastPLAYERSTATE(const NameBoolean &i_Variator) const;
  void BroadcastPLAYERSTATE() const;
  void UnicastUpdatePLAYERSTATEByplayerindex(const std::string &i_PlayerName,
        const size_t& playerindex) const;
  void VaricastUpdatePLAYERSTATEByplayerindex(const NameBoolean &i_Variator,
        const size_t& playerindex) const;
  void BroadcastUpdatePLAYERSTATEByplayerindex(
        const size_t& playerindex) const;

  void UnicastPLAYEROKSTARTUP(const std::string &i_PlayerName) const;
  void VaricastPLAYEROKSTARTUP(const NameBoolean &i_Variator) const;
  void BroadcastPLAYEROKSTARTUP() const;
  void UnicastUpdatePLAYEROKSTARTUPByplayerindex(const std::string &i_PlayerName,
        const size_t& playerindex) const;
  void VaricastUpdatePLAYEROKSTARTUPByplayerindex(const NameBoolean &i_Variator,
        const size_t& playerindex) const;
  void BroadcastUpdatePLAYEROKSTARTUPByplayerindex(
        const size_t& playerindex) const;

  void UnicastLEGALACTION(const std::string &i_PlayerName,const std::string& i_ActionName) const;
  void VaricastLEGALACTION(const NameBoolean &i_Variator,const std::string& i_ActionName) const;
  void BroadcastLEGALACTION(const std::string& i_ActionName) const;

  void UnicastITEMSTOCK(const std::string &i_PlayerName) const;
  void VaricastITEMSTOCK(const NameBoolean &i_Variator) const;
  void BroadcastITEMSTOCK() const;
  void UnicastUpdateITEMSTOCKByitemType(const std::string &i_PlayerName,
        const int& itemType) const;
  void VaricastUpdateITEMSTOCKByitemType(const NameBoolean &i_Variator,
        const int& itemType) const;
  void BroadcastUpdateITEMSTOCKByitemType(
        const int& itemType) const;

  void UnicastMESSAGE(const std::string &i_PlayerName,const std::string& i_MessageText) const;
  void VaricastMESSAGE(const NameBoolean &i_Variator,const std::string& i_MessageText) const;
  void BroadcastMESSAGE(const std::string& i_MessageText) const;

  void UnicastPURCHASEPRICE(const std::string &i_PlayerName) const;
  void VaricastPURCHASEPRICE(const NameBoolean &i_Variator) const;
  void BroadcastPURCHASEPRICE() const;

  void UnicastPLAYERITEMS(const std::string &i_PlayerName) const;
  void VaricastPLAYERITEMS(const NameBoolean &i_Variator) const;
  void BroadcastPLAYERITEMS() const;
  void UnicastUpdatePLAYERITEMSByplayerindex(const std::string &i_PlayerName,
        const size_t& playerindex) const;
  void VaricastUpdatePLAYERITEMSByplayerindex(const NameBoolean &i_Variator,
        const size_t& playerindex) const;
  void BroadcastUpdatePLAYERITEMSByplayerindex(
        const size_t& playerindex) const;
  void UnicastUpdatePLAYERITEMSByplayerindexAndByitemindex(const std::string &i_PlayerName,
        const size_t& playerindex,const size_t& itemindex) const;
  void VaricastUpdatePLAYERITEMSByplayerindexAndByitemindex(const NameBoolean &i_Variator,
        const size_t& playerindex,const size_t& itemindex) const;
  void BroadcastUpdatePLAYERITEMSByplayerindexAndByitemindex(
        const size_t& playerindex,const size_t& itemindex) const;

  void SingleCastPLAYERCARDS(const size_t& playerindex) const;
  void AllCastPLAYERCARDS() const;

  void UnicastMEGAREQUEST(const std::string &i_PlayerName) const;
  void VaricastMEGAREQUEST(const NameBoolean &i_Variator) const;
  void BroadcastMEGAREQUEST() const;

  void UnicastCURTURN(const std::string &i_PlayerName) const;
  void VaricastCURTURN(const NameBoolean &i_Variator) const;
  void BroadcastCURTURN() const;

  void UnicastTURNORDER(const std::string &i_PlayerName) const;
  void VaricastTURNORDER(const NameBoolean &i_Variator) const;
  void BroadcastTURNORDER() const;
  void UnicastUpdateTURNORDERByplayerindex(const std::string &i_PlayerName,
        const size_t& playerindex) const;
  void VaricastUpdateTURNORDERByplayerindex(const NameBoolean &i_Variator,
        const size_t& playerindex) const;
  void BroadcastUpdateTURNORDERByplayerindex(
        const size_t& playerindex) const;

  void SingleCastIAM(const size_t& playerindex) const;
  void AllCastIAM() const;

  void UnicastRESET(const std::string &i_PlayerName) const;
  void VaricastRESET(const NameBoolean &i_Variator) const;
  void BroadcastRESET() const;

  void UnicastNEWSTATE(const std::string &i_PlayerName,const std::string& i_EventName,const std::string& i_EventDesc) const;
  void VaricastNEWSTATE(const NameBoolean &i_Variator,const std::string& i_EventName,const std::string& i_EventDesc) const;
  void BroadcastNEWSTATE(const std::string& i_EventName,const std::string& i_EventDesc) const;

  void UnicastPURCHASEDATA(const std::string &i_PlayerName) const;
  void VaricastPURCHASEDATA(const NameBoolean &i_Variator) const;
  void BroadcastPURCHASEDATA() const;
  void UnicastUpdatePURCHASEDATAByshipIndex(const std::string &i_PlayerName,
        const size_t& shipIndex) const;
  void VaricastUpdatePURCHASEDATAByshipIndex(const NameBoolean &i_Variator,
        const size_t& shipIndex) const;
  void BroadcastUpdatePURCHASEDATAByshipIndex(
        const size_t& shipIndex) const;

private:
  template<class T_ArgType> std::string UnCommaStringify(const T_ArgType &i_arg) const
  {
    std::ostringstream oss;
    oss << i_arg;
    return UnComma(oss.str());
  }

  std::string MakeCOMMODITYSTOCKMessage(const int& commType) const;
  std::string MakePLAYERFACTORIESMessage(const size_t& playerindex) const;
  std::string MakePLAYERMessage(const size_t& playerindex) const;
  std::string MakePLAYERCARDBACKSMessage(const size_t& playerindex) const;
  std::string MakeSPENDSMessage(const size_t& spendIndex) const;
  std::string MakeERRORMessage(const std::string& i_ErrorText) const;
  std::string MakeSUPPLYSHIPMessage(const size_t& shipIndex) const;
  std::string MakePLAYERWINSMessage(const size_t& i_PId) const;
  std::string MakeOPTIONSMessage() const;
  std::string MakeBIDITEMMessage() const;
  std::string MakePLAYERPURCHASEINFOMessage(const size_t& playerindex) const;
  std::string MakePLAYERSTATEMessage(const size_t& playerindex) const;
  std::string MakePLAYEROKSTARTUPMessage(const size_t& playerindex) const;
  std::string MakeLEGALACTIONMessage(const std::string& i_ActionName) const;
  std::string MakeITEMSTOCKMessage(const int& itemType) const;
  std::string MakeMESSAGEMessage(const std::string& i_MessageText) const;
  std::string MakePURCHASEPRICEMessage() const;
  std::string MakePLAYERITEMSMessage(const size_t& playerindex,const size_t& itemindex) const;
  std::string MakePLAYERCARDSMessage(const size_t& playerindex) const;
  std::string MakeMEGAREQUESTMessage() const;
  std::string MakeCURTURNMessage() const;
  std::string MakeTURNORDERMessage(const size_t& playerindex) const;
  std::string MakeIAMMessage(const size_t& playerindex) const;
  std::string MakeRESETMessage() const;
  std::string MakeNEWSTATEMessage(const std::string& i_EventName,const std::string& i_EventDesc) const;
  std::string MakePURCHASEDATAMessage(const size_t& shipIndex) const;
};
class PASS_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class FILLSHIP_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class JOIN_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class OPENBID_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class BUYOREFACTORIES_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class DONTLIKEOPTIONS_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class DISCARD_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class PURCHASEBID_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class AUTOSKIPDISCARD_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class BUYTITANIUMFACTORIES_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class DETERMINETURNORDER_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class DONEMANNING_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class VALIDATESETUP_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class BUYWATERFACTORIES_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class ENDDISCARDTOTURNEND_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class COMMODITYDONETOPURCHASE_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class UNJOIN_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class DONE_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class NEWTURN_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class STARTGAME_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class STANDARDBASIC_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class BID_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class STARTPURCHASE_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class COMMODITYDONETODISCARD_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class NUMMEGAS_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class STARTDISCARD_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class MANUALDISCARD_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class PASSOUT_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class ENDMANNINGTOTURNEND_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class LIKEOPTIONS_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class OPTIONS_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class ENDMANNINGTODISCARD_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class BUYNEWCHEMFACTORIES_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class STARTMANNING_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class BUYROBOTS_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class INTERNALIZE_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class ALTERMANNING_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class ENDDISCARDTOPURCHASE_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class BUYRESEARCHFACTORIES_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class FINALDONE_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class WATERMULLIGAN_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class INITCOMMODITY_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class STANDARDEXPERT_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class BUYMEN_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class PURCHASEDONE_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class GAMEOVER_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class REQUESTMEGA_Transition : public Transition<OutpostGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const OutpostGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,OutpostGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};

class OutpostStateMachine : public StateMachine<OutpostGameInfo>
{
public:
  OutpostStateMachine();
};

#endif

