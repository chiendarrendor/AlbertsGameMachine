#ifndef AOREVENTGENERATOR
#define AOREVENTGENERATOR
#include "OutputPort.hpp"
#include "ServerGameInfo.hpp"
class AOREventGenerator
{
public:
  AOREventGenerator(OutputPort &i_OutputPort,const ServerGameInfo &i_rSGI);

  void UnicastMISERY(const std::string &i_PlayerName, const std::string &i_Capital, const int &i_MiseryIndex) const;
  void BroadcastMISERY(const std::string &i_Capital, const int &i_MiseryIndex) const;
  void VaricastMISERY(const NameBoolean &i_Variator, const std::string &i_Capital, const int &i_MiseryIndex) const;

  void UnicastHANDCARD(const std::string &i_PlayerName, const std::string &i_CardName, const int &i_Num) const;
  void BroadcastHANDCARD(const std::string &i_CardName, const int &i_Num) const;
  void VaricastHANDCARD(const NameBoolean &i_Variator, const std::string &i_CardName, const int &i_Num) const;

  void UnicastPLAYER(const std::string &i_PlayerName, const std::string &i_PName, const int &i_PlayerNum) const;
  void BroadcastPLAYER(const std::string &i_PName, const int &i_PlayerNum) const;
  void VaricastPLAYER(const NameBoolean &i_Variator, const std::string &i_PName, const int &i_PlayerNum) const;

  void UnicastADVANCESTATUS(const std::string &i_PlayerName, const std::string &i_Capital, const char &i_Advance, const std::string &i_Status, const int &i_Cost) const;
  void BroadcastADVANCESTATUS(const std::string &i_Capital, const char &i_Advance, const std::string &i_Status, const int &i_Cost) const;
  void VaricastADVANCESTATUS(const NameBoolean &i_Variator, const std::string &i_Capital, const char &i_Advance, const std::string &i_Status, const int &i_Cost) const;

  void UnicastPLAYERCAPITAL(const std::string &i_PlayerName, const int &i_PlayerNum, const std::string &i_CapitalName) const;
  void BroadcastPLAYERCAPITAL(const int &i_PlayerNum, const std::string &i_CapitalName) const;
  void VaricastPLAYERCAPITAL(const NameBoolean &i_Variator, const int &i_PlayerNum, const std::string &i_CapitalName) const;

  void UnicastCOMMODITYNUM(const std::string &i_PlayerName, const std::string &i_Commodity, const std::string &i_OwnerCapital, const int &i_NumSpaces, const int &i_NumNewSpaces) const;
  void BroadcastCOMMODITYNUM(const std::string &i_Commodity, const std::string &i_OwnerCapital, const int &i_NumSpaces, const int &i_NumNewSpaces) const;
  void VaricastCOMMODITYNUM(const NameBoolean &i_Variator, const std::string &i_Commodity, const std::string &i_OwnerCapital, const int &i_NumSpaces, const int &i_NumNewSpaces) const;

  void UnicastCASH(const std::string &i_PlayerName, const int &i_PlayerNum, const int &i_CurCash) const;
  void BroadcastCASH(const int &i_PlayerNum, const int &i_CurCash) const;
  void VaricastCASH(const NameBoolean &i_Variator, const int &i_PlayerNum, const int &i_CurCash) const;

  void UnicastTURNLOG(const std::string &i_PlayerName, const int &i_PlayerNum, const int &i_Total, const int &i_Turn, const int &i_Tokens, const int &i_Cash) const;
  void BroadcastTURNLOG(const int &i_PlayerNum, const int &i_Total, const int &i_Turn, const int &i_Tokens, const int &i_Cash) const;
  void VaricastTURNLOG(const NameBoolean &i_Variator, const int &i_PlayerNum, const int &i_Total, const int &i_Turn, const int &i_Tokens, const int &i_Cash) const;

  void UnicastNUMCARDS(const std::string &i_PlayerName, const int &i_PlayerNum, const int &i_NumCards) const;
  void BroadcastNUMCARDS(const int &i_PlayerNum, const int &i_NumCards) const;
  void VaricastNUMCARDS(const NameBoolean &i_Variator, const int &i_PlayerNum, const int &i_NumCards) const;

  void UnicastCAPITALS(const std::string &i_PlayerName, const std::string &i_Capitals) const;
  void BroadcastCAPITALS(const std::string &i_Capitals) const;
  void VaricastCAPITALS(const NameBoolean &i_Variator, const std::string &i_Capitals) const;

  void UnicastERROR(const std::string &i_PlayerName, const std::string &i_ErrorText) const;
  void BroadcastERROR(const std::string &i_ErrorText) const;
  void VaricastERROR(const NameBoolean &i_Variator, const std::string &i_ErrorText) const;

  void UnicastEPOCHSTATUS(const std::string &i_PlayerName, const int &i_TurnNum, const int &i_CurEpoch, const int &i_CardsInDeck, const int &i_NumEpochs) const;
  void BroadcastEPOCHSTATUS(const int &i_TurnNum, const int &i_CurEpoch, const int &i_CardsInDeck, const int &i_NumEpochs) const;
  void VaricastEPOCHSTATUS(const NameBoolean &i_Variator, const int &i_TurnNum, const int &i_CurEpoch, const int &i_CardsInDeck, const int &i_NumEpochs) const;

  void UnicastSHIP(const std::string &i_PlayerName, const std::string &i_Capital, const int &i_ShipType, const int &i_ShipNum) const;
  void BroadcastSHIP(const std::string &i_Capital, const int &i_ShipType, const int &i_ShipNum) const;
  void VaricastSHIP(const NameBoolean &i_Variator, const std::string &i_Capital, const int &i_ShipType, const int &i_ShipNum) const;

  void UnicastLEGALACTION(const std::string &i_PlayerName, const std::string &i_ActionName) const;
  void BroadcastLEGALACTION(const std::string &i_ActionName) const;
  void VaricastLEGALACTION(const NameBoolean &i_Variator, const std::string &i_ActionName) const;

  void UnicastMESSAGE(const std::string &i_PlayerName, const std::string &i_MessageText) const;
  void BroadcastMESSAGE(const std::string &i_MessageText) const;
  void VaricastMESSAGE(const NameBoolean &i_Variator, const std::string &i_MessageText) const;

  void UnicastNUMSTOCK(const std::string &i_PlayerName, const int &i_PlayerNum, const int &i_DominationsInStock, const int &i_DominationsOnBoard, const int &i_TokensInStock, const int &i_TokensOnBoard, const int &i_ExpansionTokens) const;
  void BroadcastNUMSTOCK(const int &i_PlayerNum, const int &i_DominationsInStock, const int &i_DominationsOnBoard, const int &i_TokensInStock, const int &i_TokensOnBoard, const int &i_ExpansionTokens) const;
  void VaricastNUMSTOCK(const NameBoolean &i_Variator, const int &i_PlayerNum, const int &i_DominationsInStock, const int &i_DominationsOnBoard, const int &i_TokensInStock, const int &i_TokensOnBoard, const int &i_ExpansionTokens) const;

  void UnicastSURPLUSSHORTAGE(const std::string &i_PlayerName, const std::string &i_Commodity, const std::string &i_State) const;
  void BroadcastSURPLUSSHORTAGE(const std::string &i_Commodity, const std::string &i_State) const;
  void VaricastSURPLUSSHORTAGE(const NameBoolean &i_Variator, const std::string &i_Commodity, const std::string &i_State) const;

  void UnicastSPACEEMPTY(const std::string &i_PlayerName, const std::string &i_SpaceName) const;
  void BroadcastSPACEEMPTY(const std::string &i_SpaceName) const;
  void VaricastSPACEEMPTY(const NameBoolean &i_Variator, const std::string &i_SpaceName) const;

  void UnicastDOMINANCE(const std::string &i_PlayerName, const std::string &i_SpaceName, const std::string &i_OwnerCapital, const std::string &i_Age) const;
  void BroadcastDOMINANCE(const std::string &i_SpaceName, const std::string &i_OwnerCapital, const std::string &i_Age) const;
  void VaricastDOMINANCE(const NameBoolean &i_Variator, const std::string &i_SpaceName, const std::string &i_OwnerCapital, const std::string &i_Age) const;

  void UnicastCAPITALBID(const std::string &i_PlayerName, const int &i_PlayerNum, const int &i_Bid) const;
  void BroadcastCAPITALBID(const int &i_PlayerNum, const int &i_Bid) const;
  void VaricastCAPITALBID(const NameBoolean &i_Variator, const int &i_PlayerNum, const int &i_Bid) const;

  void UnicastRESET(const std::string &i_PlayerName) const;
  void BroadcastRESET() const;
  void VaricastRESET(const NameBoolean &i_Variator) const;

  void UnicastCAPITALBIDS(const std::string &i_PlayerName, const std::string &i_CapitalBids) const;
  void BroadcastCAPITALBIDS(const std::string &i_CapitalBids) const;
  void VaricastCAPITALBIDS(const NameBoolean &i_Variator, const std::string &i_CapitalBids) const;

  void UnicastTOKEN(const std::string &i_PlayerName, const std::string &i_SpaceName, const int &i_TokenNum, const std::string &i_OwnerCapital, const std::string &i_Age) const;
  void BroadcastTOKEN(const std::string &i_SpaceName, const int &i_TokenNum, const std::string &i_OwnerCapital, const std::string &i_Age) const;
  void VaricastTOKEN(const NameBoolean &i_Variator, const std::string &i_SpaceName, const int &i_TokenNum, const std::string &i_OwnerCapital, const std::string &i_Age) const;

  void UnicastNEWSTATE(const std::string &i_PlayerName, const std::string &i_EventName) const;
  void BroadcastNEWSTATE(const std::string &i_EventName) const;
  void VaricastNEWSTATE(const NameBoolean &i_Variator, const std::string &i_EventName) const;

private:
  OutputPort &m_OutputPort;
  const ServerGameInfo &m_ServerGameInfo;
  template<class T_ArgType> std::string UnCommaStringify(const T_ArgType &i_arg) const
  {
    std::ostringstream oss;
    oss << i_arg;
    return UnComma(oss.str());
  }
  std::string MakeMISERYMessage(const std::string &i_Capital, const int &i_MiseryIndex) const;
  std::string MakeHANDCARDMessage(const std::string &i_CardName, const int &i_Num) const;
  std::string MakePLAYERMessage(const std::string &i_PName, const int &i_PlayerNum) const;
  std::string MakeADVANCESTATUSMessage(const std::string &i_Capital, const char &i_Advance, const std::string &i_Status, const int &i_Cost) const;
  std::string MakePLAYERCAPITALMessage(const int &i_PlayerNum, const std::string &i_CapitalName) const;
  std::string MakeCOMMODITYNUMMessage(const std::string &i_Commodity, const std::string &i_OwnerCapital, const int &i_NumSpaces, const int &i_NumNewSpaces) const;
  std::string MakeCASHMessage(const int &i_PlayerNum, const int &i_CurCash) const;
  std::string MakeTURNLOGMessage(const int &i_PlayerNum, const int &i_Total, const int &i_Turn, const int &i_Tokens, const int &i_Cash) const;
  std::string MakeNUMCARDSMessage(const int &i_PlayerNum, const int &i_NumCards) const;
  std::string MakeCAPITALSMessage(const std::string &i_Capitals) const;
  std::string MakeERRORMessage(const std::string &i_ErrorText) const;
  std::string MakeEPOCHSTATUSMessage(const int &i_TurnNum, const int &i_CurEpoch, const int &i_CardsInDeck, const int &i_NumEpochs) const;
  std::string MakeSHIPMessage(const std::string &i_Capital, const int &i_ShipType, const int &i_ShipNum) const;
  std::string MakeLEGALACTIONMessage(const std::string &i_ActionName) const;
  std::string MakeMESSAGEMessage(const std::string &i_MessageText) const;
  std::string MakeNUMSTOCKMessage(const int &i_PlayerNum, const int &i_DominationsInStock, const int &i_DominationsOnBoard, const int &i_TokensInStock, const int &i_TokensOnBoard, const int &i_ExpansionTokens) const;
  std::string MakeSURPLUSSHORTAGEMessage(const std::string &i_Commodity, const std::string &i_State) const;
  std::string MakeSPACEEMPTYMessage(const std::string &i_SpaceName) const;
  std::string MakeDOMINANCEMessage(const std::string &i_SpaceName, const std::string &i_OwnerCapital, const std::string &i_Age) const;
  std::string MakeCAPITALBIDMessage(const int &i_PlayerNum, const int &i_Bid) const;
  std::string MakeRESETMessage() const;
  std::string MakeCAPITALBIDSMessage(const std::string &i_CapitalBids) const;
  std::string MakeTOKENMessage(const std::string &i_SpaceName, const int &i_TokenNum, const std::string &i_OwnerCapital, const std::string &i_Age) const;
  std::string MakeNEWSTATEMessage(const std::string &i_EventName) const;
};

#endif
