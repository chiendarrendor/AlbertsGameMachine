#ifndef MerchantOfVenusGAMEINFOHPP
#define MerchantOfVenusGAMEINFOHPP
#include "GameInfo.hpp"
#include "MerchantOfVenusSet.hpp"
#include "MerchantOfVenusState.hpp"
#include "StateMachine.hpp"

class NameBoolean;

class MerchantOfVenusGameInfo : public GameInfo<MerchantOfVenusSet,MerchantOfVenusState>
{
public:
  MerchantOfVenusGameInfo(const MerchantOfVenusSet &i_Set,
                  const ServerGameInfo &i_ginfo,
                  OutputPort  &i_Port);
private:
  static const StateName CompleteMove;
  static const StateName TurnEnd;
  static const StateName TerminalState;
  static const StateName SelectPilotNumber;
  static const StateName InitialState;
  static const StateName SelectMove;
  static const StateName AsteroidPhase;
  static const StateName AutoTradePhaseSelect;
  static const StateName TechSelected;
  static const StateName Mulligan;
  static const StateName StartTurn;
  static const StateName TechSelect;
  static const StateName TradePhase;
  static const StateName ValidateSetup;
  static const StateName ProcessPilotNumber;
  static const StateName FinalizeMove;
  mutable int m_errorcount;
public:
  void ResetERRORCount();
  int GetERRORCount() const;
  bool JOIN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool JOIN_IsAuto(const StateName &i_StateName) const;
  bool JOIN_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool AUTOPILOT_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool AUTOPILOT_IsAuto(const StateName &i_StateName) const;
  bool AUTOPILOT_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool NOMULLIGAN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool NOMULLIGAN_IsAuto(const StateName &i_StateName) const;
  bool NOMULLIGAN_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool CANTMULLIGAN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool CANTMULLIGAN_IsAuto(const StateName &i_StateName) const;
  bool CANTMULLIGAN_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool DONTLIKEOPTIONS_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool DONTLIKEOPTIONS_IsAuto(const StateName &i_StateName) const;
  bool DONTLIKEOPTIONS_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool GAMENOTOVER_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool GAMENOTOVER_IsAuto(const StateName &i_StateName) const;
  bool GAMENOTOVER_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool SELL_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool SELL_IsAuto(const StateName &i_StateName) const;
  bool SELL_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool MULLIGAN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool MULLIGAN_IsAuto(const StateName &i_StateName) const;
  bool MULLIGAN_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool EXECUTEMOVE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool EXECUTEMOVE_IsAuto(const StateName &i_StateName) const;
  bool EXECUTEMOVE_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool VALIDATESETUP_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool VALIDATESETUP_IsAuto(const StateName &i_StateName) const;
  bool VALIDATESETUP_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool LEAVERELIC_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool LEAVERELIC_IsAuto(const StateName &i_StateName) const;
  bool LEAVERELIC_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool UNJOIN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool UNJOIN_IsAuto(const StateName &i_StateName) const;
  bool UNJOIN_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool JETTISON_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool JETTISON_IsAuto(const StateName &i_StateName) const;
  bool JETTISON_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool GOTOTECHSELECT_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool GOTOTECHSELECT_IsAuto(const StateName &i_StateName) const;
  bool GOTOTECHSELECT_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool ENDTURN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool ENDTURN_IsAuto(const StateName &i_StateName) const;
  bool ENDTURN_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool STARTGAME_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool STARTGAME_IsAuto(const StateName &i_StateName) const;
  bool STARTGAME_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool ISRELIC_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool ISRELIC_IsAuto(const StateName &i_StateName) const;
  bool ISRELIC_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool GOTOSELECTPILOTNUMBER_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool GOTOSELECTPILOTNUMBER_IsAuto(const StateName &i_StateName) const;
  bool GOTOSELECTPILOTNUMBER_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool INSPACE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool INSPACE_IsAuto(const StateName &i_StateName) const;
  bool INSPACE_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool SELECTSWITCHABLES_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool SELECTSWITCHABLES_IsAuto(const StateName &i_StateName) const;
  bool SELECTSWITCHABLES_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool ISTRADEBASE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool ISTRADEBASE_IsAuto(const StateName &i_StateName) const;
  bool ISTRADEBASE_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool SKIPTECHSELECT_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool SKIPTECHSELECT_IsAuto(const StateName &i_StateName) const;
  bool SKIPTECHSELECT_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool ENDMOVE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool ENDMOVE_IsAuto(const StateName &i_StateName) const;
  bool ENDMOVE_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool AUTOCHOOSEDEST_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool AUTOCHOOSEDEST_IsAuto(const StateName &i_StateName) const;
  bool AUTOCHOOSEDEST_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool TRADEINSHIP_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool TRADEINSHIP_IsAuto(const StateName &i_StateName) const;
  bool TRADEINSHIP_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool REJECTDESTINATION_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool REJECTDESTINATION_IsAuto(const StateName &i_StateName) const;
  bool REJECTDESTINATION_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool PICKUP_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool PICKUP_IsAuto(const StateName &i_StateName) const;
  bool PICKUP_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool SELECTDESTINATION_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool SELECTDESTINATION_IsAuto(const StateName &i_StateName) const;
  bool SELECTDESTINATION_ExecuteAction(const std::string &i_PlayerName,
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
  bool MANUALCHOOSEDEST_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool MANUALCHOOSEDEST_IsAuto(const StateName &i_StateName) const;
  bool MANUALCHOOSEDEST_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool SELECTPILOTNUMBER_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool SELECTPILOTNUMBER_IsAuto(const StateName &i_StateName) const;
  bool SELECTPILOTNUMBER_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool REDEEMIOU_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool REDEEMIOU_IsAuto(const StateName &i_StateName) const;
  bool REDEEMIOU_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool BUY_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool BUY_IsAuto(const StateName &i_StateName) const;
  bool BUY_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool DROPOFF_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool DROPOFF_IsAuto(const StateName &i_StateName) const;
  bool DROPOFF_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool GETRELIC_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool GETRELIC_IsAuto(const StateName &i_StateName) const;
  bool GETRELIC_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool GOTOCHOOSE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool GOTOCHOOSE_IsAuto(const StateName &i_StateName) const;
  bool GOTOCHOOSE_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  bool GAMEOVER_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;
  bool GAMEOVER_IsAuto(const StateName &i_StateName) const;
  bool GAMEOVER_ExecuteAction(const std::string &i_PlayerName,
                              const ActionParser &i_ap,
                              const StateName &i_StateName);
  void SendFullState(const std::string &i_PlayerName,const StateName &i_StateName) const;


  void UnicastDESTINATION(const std::string &i_PlayerName,const int& i_curspacepenalty,const bool& i_isfirst,const std::string& i_destinations) const;
  void VaricastDESTINATION(const NameBoolean &i_Variator,const int& i_curspacepenalty,const bool& i_isfirst,const std::string& i_destinations) const;
  void BroadcastDESTINATION(const int& i_curspacepenalty,const bool& i_isfirst,const std::string& i_destinations) const;

  void UnicastQBOXCONTENTS(const std::string &i_PlayerName,const std::string& i_spacename,const std::string& i_contents) const;
  void VaricastQBOXCONTENTS(const NameBoolean &i_Variator,const std::string& i_spacename,const std::string& i_contents) const;
  void BroadcastQBOXCONTENTS(const std::string& i_spacename,const std::string& i_contents) const;

  void UnicastSUBTRACTFROMTRADEBASE(const std::string &i_PlayerName,const std::string& i_raceid,const std::string& i_token) const;
  void VaricastSUBTRACTFROMTRADEBASE(const NameBoolean &i_Variator,const std::string& i_raceid,const std::string& i_token) const;
  void BroadcastSUBTRACTFROMTRADEBASE(const std::string& i_raceid,const std::string& i_token) const;

  void UnicastFACTORYOWNER(const std::string &i_PlayerName,const std::string& i_raceid,const size_t& i_playerid) const;
  void VaricastFACTORYOWNER(const NameBoolean &i_Variator,const std::string& i_raceid,const size_t& i_playerid) const;
  void BroadcastFACTORYOWNER(const std::string& i_raceid,const size_t& i_playerid) const;

  void UnicastERROR(const std::string &i_PlayerName,const std::string& i_ErrorText) const;
  void VaricastERROR(const NameBoolean &i_Variator,const std::string& i_ErrorText) const;
  void BroadcastERROR(const std::string& i_ErrorText) const;

  void UnicastCURPLAYER(const std::string &i_PlayerName) const;
  void VaricastCURPLAYER(const NameBoolean &i_Variator) const;
  void BroadcastCURPLAYER() const;

  void UnicastPLAYEROKSTARTUP(const std::string &i_PlayerName) const;
  void VaricastPLAYEROKSTARTUP(const NameBoolean &i_Variator) const;
  void BroadcastPLAYEROKSTARTUP() const;
  void UnicastUpdatePLAYEROKSTARTUPByplayerindex(const std::string &i_PlayerName,
        const size_t& playerindex) const;
  void VaricastUpdatePLAYEROKSTARTUPByplayerindex(const NameBoolean &i_Variator,
        const size_t& playerindex) const;
  void BroadcastUpdatePLAYEROKSTARTUPByplayerindex(
        const size_t& playerindex) const;

  void UnicastMESSAGE(const std::string &i_PlayerName,const std::string& i_MessageText) const;
  void VaricastMESSAGE(const NameBoolean &i_Variator,const std::string& i_MessageText) const;
  void BroadcastMESSAGE(const std::string& i_MessageText) const;

  void UnicastSWITCHABLES(const std::string &i_PlayerName,const std::string& i_switchables) const;
  void VaricastSWITCHABLES(const NameBoolean &i_Variator,const std::string& i_switchables) const;
  void BroadcastSWITCHABLES(const std::string& i_switchables) const;

  void UnicastSELECTEDSWITCHABLES(const std::string &i_PlayerName,const size_t& i_playerid,const std::string& i_switchables) const;
  void VaricastSELECTEDSWITCHABLES(const NameBoolean &i_Variator,const size_t& i_playerid,const std::string& i_switchables) const;
  void BroadcastSELECTEDSWITCHABLES(const size_t& i_playerid,const std::string& i_switchables) const;

  void UnicastADDTOTRADEBASE(const std::string &i_PlayerName,const std::string& i_raceid,const std::string& i_token,const bool& i_fromcup) const;
  void VaricastADDTOTRADEBASE(const NameBoolean &i_Variator,const std::string& i_raceid,const std::string& i_token,const bool& i_fromcup) const;
  void BroadcastADDTOTRADEBASE(const std::string& i_raceid,const std::string& i_token,const bool& i_fromcup) const;

  void UnicastMOVERROR(const std::string &i_PlayerName,const std::string& i_Error) const;
  void VaricastMOVERROR(const NameBoolean &i_Variator,const std::string& i_Error) const;
  void BroadcastMOVERROR(const std::string& i_Error) const;

  void UnicastTURNORDER(const std::string &i_PlayerName) const;
  void VaricastTURNORDER(const NameBoolean &i_Variator) const;
  void BroadcastTURNORDER() const;

  void UnicastPLAYERLOCATION(const std::string &i_PlayerName) const;
  void VaricastPLAYERLOCATION(const NameBoolean &i_Variator) const;
  void BroadcastPLAYERLOCATION() const;
  void UnicastUpdatePLAYERLOCATIONByplayerindex(const std::string &i_PlayerName,
        const size_t& playerindex) const;
  void VaricastUpdatePLAYERLOCATIONByplayerindex(const NameBoolean &i_Variator,
        const size_t& playerindex) const;
  void BroadcastUpdatePLAYERLOCATIONByplayerindex(
        const size_t& playerindex) const;

  void UnicastSUBTRACTFROMPLAYER(const std::string &i_PlayerName,const size_t& i_playerid,const std::string& i_token) const;
  void VaricastSUBTRACTFROMPLAYER(const NameBoolean &i_Variator,const size_t& i_playerid,const std::string& i_token) const;
  void BroadcastSUBTRACTFROMPLAYER(const size_t& i_playerid,const std::string& i_token) const;

  void UnicastDICE(const std::string &i_PlayerName,const std::string& i_dicestring) const;
  void VaricastDICE(const NameBoolean &i_Variator,const std::string& i_dicestring) const;
  void BroadcastDICE(const std::string& i_dicestring) const;

  void UnicastREMAININGTRANSCOUNT(const std::string &i_PlayerName,const size_t& i_playerid,const int& numbuys,const int& numsells) const;
  void VaricastREMAININGTRANSCOUNT(const NameBoolean &i_Variator,const size_t& i_playerid,const int& numbuys,const int& numsells) const;
  void BroadcastREMAININGTRANSCOUNT(const size_t& i_playerid,const int& numbuys,const int& numsells) const;

  void UnicastPLAYERRACE(const std::string &i_PlayerName) const;
  void VaricastPLAYERRACE(const NameBoolean &i_Variator) const;
  void BroadcastPLAYERRACE() const;
  void UnicastUpdatePLAYERRACEByplayerindex(const std::string &i_PlayerName,
        const size_t& playerindex) const;
  void VaricastUpdatePLAYERRACEByplayerindex(const NameBoolean &i_Variator,
        const size_t& playerindex) const;
  void BroadcastUpdatePLAYERRACEByplayerindex(
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

  void UnicastMPLEFT(const std::string &i_PlayerName,const int& i_mpleft) const;
  void VaricastMPLEFT(const NameBoolean &i_Variator,const int& i_mpleft) const;
  void BroadcastMPLEFT(const int& i_mpleft) const;

  void UnicastPILOTNUMBERCHOSEN(const std::string &i_PlayerName,const size_t& i_playerid,const size_t& i_pilotnumber,const bool& i_ispermanent) const;
  void VaricastPILOTNUMBERCHOSEN(const NameBoolean &i_Variator,const size_t& i_playerid,const size_t& i_pilotnumber,const bool& i_ispermanent) const;
  void BroadcastPILOTNUMBERCHOSEN(const size_t& i_playerid,const size_t& i_pilotnumber,const bool& i_ispermanent) const;

  void UnicastSOLARSYSTEMPRIVACY(const std::string &i_PlayerName,const std::string& i_solarsystem,const std::string& i_secrecy) const;
  void VaricastSOLARSYSTEMPRIVACY(const NameBoolean &i_Variator,const std::string& i_solarsystem,const std::string& i_secrecy) const;
  void BroadcastSOLARSYSTEMPRIVACY(const std::string& i_solarsystem,const std::string& i_secrecy) const;

  void UnicastOPTIONS(const std::string &i_PlayerName) const;
  void VaricastOPTIONS(const NameBoolean &i_Variator) const;
  void BroadcastOPTIONS() const;

  void UnicastPLAYERWINS(const std::string &i_PlayerName,const size_t& i_playerid) const;
  void VaricastPLAYERWINS(const NameBoolean &i_Variator,const size_t& i_playerid) const;
  void BroadcastPLAYERWINS(const size_t& i_playerid) const;

  void UnicastLEGALACTION(const std::string &i_PlayerName,const std::string& i_ActionName) const;
  void VaricastLEGALACTION(const NameBoolean &i_Variator,const std::string& i_ActionName) const;
  void BroadcastLEGALACTION(const std::string& i_ActionName) const;

  void UnicastADDTOPLAYER(const std::string &i_PlayerName,const size_t& i_playerid,const std::string& i_token) const;
  void VaricastADDTOPLAYER(const NameBoolean &i_Variator,const size_t& i_playerid,const std::string& i_token) const;
  void BroadcastADDTOPLAYER(const size_t& i_playerid,const std::string& i_token) const;

  void UnicastQBOXPRIVACY(const std::string &i_PlayerName,const std::string& i_spacename,const std::string& i_secrecy) const;
  void VaricastQBOXPRIVACY(const NameBoolean &i_Variator,const std::string& i_spacename,const std::string& i_secrecy) const;
  void BroadcastQBOXPRIVACY(const std::string& i_spacename,const std::string& i_secrecy) const;

  void UnicastPILOTNUMBERCHOICE(const std::string &i_PlayerName,const std::string& i_pnumbers) const;
  void VaricastPILOTNUMBERCHOICE(const NameBoolean &i_Variator,const std::string& i_pnumbers) const;
  void BroadcastPILOTNUMBERCHOICE(const std::string& i_pnumbers) const;

  void UnicastBASELOCATION(const std::string &i_PlayerName,const std::string& i_solarsystem,const std::string& i_raceid) const;
  void VaricastBASELOCATION(const NameBoolean &i_Variator,const std::string& i_solarsystem,const std::string& i_raceid) const;
  void BroadcastBASELOCATION(const std::string& i_solarsystem,const std::string& i_raceid) const;

  void UnicastPLAYERMONEY(const std::string &i_PlayerName) const;
  void VaricastPLAYERMONEY(const NameBoolean &i_Variator) const;
  void BroadcastPLAYERMONEY() const;
  void UnicastUpdatePLAYERMONEYByplayerindex(const std::string &i_PlayerName,
        const size_t& playerindex) const;
  void VaricastUpdatePLAYERMONEYByplayerindex(const NameBoolean &i_Variator,
        const size_t& playerindex) const;
  void BroadcastUpdatePLAYERMONEYByplayerindex(
        const size_t& playerindex) const;

  void UnicastAGEPLAYER(const std::string &i_PlayerName,const size_t& i_playerid) const;
  void VaricastAGEPLAYER(const NameBoolean &i_Variator,const size_t& i_playerid) const;
  void BroadcastAGEPLAYER(const size_t& i_playerid) const;

  void UnicastRESET(const std::string &i_PlayerName) const;
  void VaricastRESET(const NameBoolean &i_Variator) const;
  void BroadcastRESET() const;

  void UnicastIAM(const std::string &i_PlayerName,const size_t& i_PId) const;
  void VaricastIAM(const NameBoolean &i_Variator,const size_t& i_PId) const;
  void BroadcastIAM(const size_t& i_PId) const;

  void UnicastNEWSTATE(const std::string &i_PlayerName,const std::string& i_EventName,const std::string& i_EventDesc) const;
  void VaricastNEWSTATE(const NameBoolean &i_Variator,const std::string& i_EventName,const std::string& i_EventDesc) const;
  void BroadcastNEWSTATE(const std::string& i_EventName,const std::string& i_EventDesc) const;

private:
  template<class T_ArgType> std::string UnCommaStringify(const T_ArgType &i_arg) const
  {
    std::ostringstream oss;
    oss << i_arg;
    return UnComma(oss.str());
  }

  std::string MakeDESTINATIONMessage(const int& i_curspacepenalty,const bool& i_isfirst,const std::string& i_destinations) const;
  std::string MakeQBOXCONTENTSMessage(const std::string& i_spacename,const std::string& i_contents) const;
  std::string MakeSUBTRACTFROMTRADEBASEMessage(const std::string& i_raceid,const std::string& i_token) const;
  std::string MakeFACTORYOWNERMessage(const std::string& i_raceid,const size_t& i_playerid) const;
  std::string MakeERRORMessage(const std::string& i_ErrorText) const;
  std::string MakeCURPLAYERMessage() const;
  std::string MakePLAYEROKSTARTUPMessage(const size_t& playerindex) const;
  std::string MakeMESSAGEMessage(const std::string& i_MessageText) const;
  std::string MakeSWITCHABLESMessage(const std::string& i_switchables) const;
  std::string MakeSELECTEDSWITCHABLESMessage(const size_t& i_playerid,const std::string& i_switchables) const;
  std::string MakeADDTOTRADEBASEMessage(const std::string& i_raceid,const std::string& i_token,const bool& i_fromcup) const;
  std::string MakeMOVERRORMessage(const std::string& i_Error) const;
  std::string MakeTURNORDERMessage() const;
  std::string MakePLAYERLOCATIONMessage(const size_t& playerindex) const;
  std::string MakeSUBTRACTFROMPLAYERMessage(const size_t& i_playerid,const std::string& i_token) const;
  std::string MakeDICEMessage(const std::string& i_dicestring) const;
  std::string MakeREMAININGTRANSCOUNTMessage(const size_t& i_playerid,const int& numbuys,const int& numsells) const;
  std::string MakePLAYERRACEMessage(const size_t& playerindex) const;
  std::string MakePLAYERMessage(const size_t& playerindex) const;
  std::string MakeMPLEFTMessage(const int& i_mpleft) const;
  std::string MakePILOTNUMBERCHOSENMessage(const size_t& i_playerid,const size_t& i_pilotnumber,const bool& i_ispermanent) const;
  std::string MakeSOLARSYSTEMPRIVACYMessage(const std::string& i_solarsystem,const std::string& i_secrecy) const;
  std::string MakeOPTIONSMessage() const;
  std::string MakePLAYERWINSMessage(const size_t& i_playerid) const;
  std::string MakeLEGALACTIONMessage(const std::string& i_ActionName) const;
  std::string MakeADDTOPLAYERMessage(const size_t& i_playerid,const std::string& i_token) const;
  std::string MakeQBOXPRIVACYMessage(const std::string& i_spacename,const std::string& i_secrecy) const;
  std::string MakePILOTNUMBERCHOICEMessage(const std::string& i_pnumbers) const;
  std::string MakeBASELOCATIONMessage(const std::string& i_solarsystem,const std::string& i_raceid) const;
  std::string MakePLAYERMONEYMessage(const size_t& playerindex) const;
  std::string MakeAGEPLAYERMessage(const size_t& i_playerid) const;
  std::string MakeRESETMessage() const;
  std::string MakeIAMMessage(const size_t& i_PId) const;
  std::string MakeNEWSTATEMessage(const std::string& i_EventName,const std::string& i_EventDesc) const;
};
class JOIN_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class AUTOPILOT_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class NOMULLIGAN_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class CANTMULLIGAN_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class DONTLIKEOPTIONS_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class GAMENOTOVER_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class SELL_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class MULLIGAN_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class EXECUTEMOVE_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class VALIDATESETUP_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class LEAVERELIC_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class UNJOIN_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class JETTISON_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class GOTOTECHSELECT_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class ENDTURN_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class STARTGAME_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class ISRELIC_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class GOTOSELECTPILOTNUMBER_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class INSPACE_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class SELECTSWITCHABLES_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class ISTRADEBASE_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class SKIPTECHSELECT_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class ENDMOVE_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class AUTOCHOOSEDEST_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class TRADEINSHIP_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class REJECTDESTINATION_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class PICKUP_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class SELECTDESTINATION_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class LIKEOPTIONS_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class OPTIONS_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class MANUALCHOOSEDEST_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class SELECTPILOTNUMBER_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class REDEEMIOU_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class BUY_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class DROPOFF_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class GETRELIC_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class GOTOCHOOSE_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};
class GAMEOVER_Transition : public Transition<MerchantOfVenusGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool IsAuto(const StateName &i_StateName,const MerchantOfVenusGameInfo &i_gi) const;
  virtual bool ExecuteAction(const StateName &i_StateName,MerchantOfVenusGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const;
  virtual std::string GetName() const;
};

class MerchantOfVenusStateMachine : public StateMachine<MerchantOfVenusGameInfo>
{
public:
  MerchantOfVenusStateMachine();
};

#endif

