#include <sstream>
#include <set>
#include "StringUtilities.hpp"
#include "GameBoxDLL.hpp"
#include "GameInfo.hpp"
#include "StateMachine.hpp"
#include "DLLGame.hpp"

#include "AORSet.hpp"
#include "AORState.hpp"
#include "AOREventGenerator.hpp"

AOREventGenerator::AOREventGenerator(OutputPort &i_OutputPort,const ServerGameInfo &i_rSGI) : 
    m_OutputPort(i_OutputPort),m_ServerGameInfo(i_rSGI) {};

void AOREventGenerator::UnicastMISERY(const std::string &i_PlayerName, const std::string &i_Capital, const int &i_MiseryIndex) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeMISERYMessage(i_Capital, i_MiseryIndex));
}
void AOREventGenerator::BroadcastMISERY(const std::string &i_Capital, const int &i_MiseryIndex) const
{
  m_OutputPort.BroadCast(MakeMISERYMessage(i_Capital, i_MiseryIndex));
}
void AOREventGenerator::VaricastMISERY(const NameBoolean &i_Variator, const std::string &i_Capital, const int &i_MiseryIndex) const
{
  m_OutputPort.VariCast(i_Variator,MakeMISERYMessage(i_Capital, i_MiseryIndex));
}

void AOREventGenerator::UnicastHANDCARD(const std::string &i_PlayerName, const std::string &i_CardName, const int &i_Num) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeHANDCARDMessage(i_CardName, i_Num));
}
void AOREventGenerator::BroadcastHANDCARD(const std::string &i_CardName, const int &i_Num) const
{
  m_OutputPort.BroadCast(MakeHANDCARDMessage(i_CardName, i_Num));
}
void AOREventGenerator::VaricastHANDCARD(const NameBoolean &i_Variator, const std::string &i_CardName, const int &i_Num) const
{
  m_OutputPort.VariCast(i_Variator,MakeHANDCARDMessage(i_CardName, i_Num));
}

void AOREventGenerator::UnicastPLAYER(const std::string &i_PlayerName, const std::string &i_PName, const int &i_PlayerNum) const
{
  m_OutputPort.UniCast(i_PlayerName,MakePLAYERMessage(i_PName, i_PlayerNum));
}
void AOREventGenerator::BroadcastPLAYER(const std::string &i_PName, const int &i_PlayerNum) const
{
  m_OutputPort.BroadCast(MakePLAYERMessage(i_PName, i_PlayerNum));
}
void AOREventGenerator::VaricastPLAYER(const NameBoolean &i_Variator, const std::string &i_PName, const int &i_PlayerNum) const
{
  m_OutputPort.VariCast(i_Variator,MakePLAYERMessage(i_PName, i_PlayerNum));
}

void AOREventGenerator::UnicastADVANCESTATUS(const std::string &i_PlayerName, const std::string &i_Capital, const char &i_Advance, const std::string &i_Status, const int &i_Cost) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeADVANCESTATUSMessage(i_Capital, i_Advance, i_Status, i_Cost));
}
void AOREventGenerator::BroadcastADVANCESTATUS(const std::string &i_Capital, const char &i_Advance, const std::string &i_Status, const int &i_Cost) const
{
  m_OutputPort.BroadCast(MakeADVANCESTATUSMessage(i_Capital, i_Advance, i_Status, i_Cost));
}
void AOREventGenerator::VaricastADVANCESTATUS(const NameBoolean &i_Variator, const std::string &i_Capital, const char &i_Advance, const std::string &i_Status, const int &i_Cost) const
{
  m_OutputPort.VariCast(i_Variator,MakeADVANCESTATUSMessage(i_Capital, i_Advance, i_Status, i_Cost));
}

void AOREventGenerator::UnicastPLAYERCAPITAL(const std::string &i_PlayerName, const int &i_PlayerNum, const std::string &i_CapitalName) const
{
  m_OutputPort.UniCast(i_PlayerName,MakePLAYERCAPITALMessage(i_PlayerNum, i_CapitalName));
}
void AOREventGenerator::BroadcastPLAYERCAPITAL(const int &i_PlayerNum, const std::string &i_CapitalName) const
{
  m_OutputPort.BroadCast(MakePLAYERCAPITALMessage(i_PlayerNum, i_CapitalName));
}
void AOREventGenerator::VaricastPLAYERCAPITAL(const NameBoolean &i_Variator, const int &i_PlayerNum, const std::string &i_CapitalName) const
{
  m_OutputPort.VariCast(i_Variator,MakePLAYERCAPITALMessage(i_PlayerNum, i_CapitalName));
}

void AOREventGenerator::UnicastCOMMODITYNUM(const std::string &i_PlayerName, const std::string &i_Commodity, const std::string &i_OwnerCapital, const int &i_NumSpaces, const int &i_NumNewSpaces) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeCOMMODITYNUMMessage(i_Commodity, i_OwnerCapital, i_NumSpaces, i_NumNewSpaces));
}
void AOREventGenerator::BroadcastCOMMODITYNUM(const std::string &i_Commodity, const std::string &i_OwnerCapital, const int &i_NumSpaces, const int &i_NumNewSpaces) const
{
  m_OutputPort.BroadCast(MakeCOMMODITYNUMMessage(i_Commodity, i_OwnerCapital, i_NumSpaces, i_NumNewSpaces));
}
void AOREventGenerator::VaricastCOMMODITYNUM(const NameBoolean &i_Variator, const std::string &i_Commodity, const std::string &i_OwnerCapital, const int &i_NumSpaces, const int &i_NumNewSpaces) const
{
  m_OutputPort.VariCast(i_Variator,MakeCOMMODITYNUMMessage(i_Commodity, i_OwnerCapital, i_NumSpaces, i_NumNewSpaces));
}

void AOREventGenerator::UnicastCASH(const std::string &i_PlayerName, const int &i_PlayerNum, const int &i_CurCash) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeCASHMessage(i_PlayerNum, i_CurCash));
}
void AOREventGenerator::BroadcastCASH(const int &i_PlayerNum, const int &i_CurCash) const
{
  m_OutputPort.BroadCast(MakeCASHMessage(i_PlayerNum, i_CurCash));
}
void AOREventGenerator::VaricastCASH(const NameBoolean &i_Variator, const int &i_PlayerNum, const int &i_CurCash) const
{
  m_OutputPort.VariCast(i_Variator,MakeCASHMessage(i_PlayerNum, i_CurCash));
}

void AOREventGenerator::UnicastTURNLOG(const std::string &i_PlayerName, const int &i_PlayerNum, const int &i_Total, const int &i_Turn, const int &i_Tokens, const int &i_Cash) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeTURNLOGMessage(i_PlayerNum, i_Total, i_Turn, i_Tokens, i_Cash));
}
void AOREventGenerator::BroadcastTURNLOG(const int &i_PlayerNum, const int &i_Total, const int &i_Turn, const int &i_Tokens, const int &i_Cash) const
{
  m_OutputPort.BroadCast(MakeTURNLOGMessage(i_PlayerNum, i_Total, i_Turn, i_Tokens, i_Cash));
}
void AOREventGenerator::VaricastTURNLOG(const NameBoolean &i_Variator, const int &i_PlayerNum, const int &i_Total, const int &i_Turn, const int &i_Tokens, const int &i_Cash) const
{
  m_OutputPort.VariCast(i_Variator,MakeTURNLOGMessage(i_PlayerNum, i_Total, i_Turn, i_Tokens, i_Cash));
}

void AOREventGenerator::UnicastNUMCARDS(const std::string &i_PlayerName, const int &i_PlayerNum, const int &i_NumCards) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeNUMCARDSMessage(i_PlayerNum, i_NumCards));
}
void AOREventGenerator::BroadcastNUMCARDS(const int &i_PlayerNum, const int &i_NumCards) const
{
  m_OutputPort.BroadCast(MakeNUMCARDSMessage(i_PlayerNum, i_NumCards));
}
void AOREventGenerator::VaricastNUMCARDS(const NameBoolean &i_Variator, const int &i_PlayerNum, const int &i_NumCards) const
{
  m_OutputPort.VariCast(i_Variator,MakeNUMCARDSMessage(i_PlayerNum, i_NumCards));
}

void AOREventGenerator::UnicastCAPITALS(const std::string &i_PlayerName, const std::string &i_Capitals) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeCAPITALSMessage(i_Capitals));
}
void AOREventGenerator::BroadcastCAPITALS(const std::string &i_Capitals) const
{
  m_OutputPort.BroadCast(MakeCAPITALSMessage(i_Capitals));
}
void AOREventGenerator::VaricastCAPITALS(const NameBoolean &i_Variator, const std::string &i_Capitals) const
{
  m_OutputPort.VariCast(i_Variator,MakeCAPITALSMessage(i_Capitals));
}

void AOREventGenerator::UnicastERROR(const std::string &i_PlayerName, const std::string &i_ErrorText) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeERRORMessage(i_ErrorText));
}
void AOREventGenerator::BroadcastERROR(const std::string &i_ErrorText) const
{
  m_OutputPort.BroadCast(MakeERRORMessage(i_ErrorText));
}
void AOREventGenerator::VaricastERROR(const NameBoolean &i_Variator, const std::string &i_ErrorText) const
{
  m_OutputPort.VariCast(i_Variator,MakeERRORMessage(i_ErrorText));
}

void AOREventGenerator::UnicastEPOCHSTATUS(const std::string &i_PlayerName, const int &i_TurnNum, const int &i_CurEpoch, const int &i_CardsInDeck, const int &i_NumEpochs) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeEPOCHSTATUSMessage(i_TurnNum, i_CurEpoch, i_CardsInDeck, i_NumEpochs));
}
void AOREventGenerator::BroadcastEPOCHSTATUS(const int &i_TurnNum, const int &i_CurEpoch, const int &i_CardsInDeck, const int &i_NumEpochs) const
{
  m_OutputPort.BroadCast(MakeEPOCHSTATUSMessage(i_TurnNum, i_CurEpoch, i_CardsInDeck, i_NumEpochs));
}
void AOREventGenerator::VaricastEPOCHSTATUS(const NameBoolean &i_Variator, const int &i_TurnNum, const int &i_CurEpoch, const int &i_CardsInDeck, const int &i_NumEpochs) const
{
  m_OutputPort.VariCast(i_Variator,MakeEPOCHSTATUSMessage(i_TurnNum, i_CurEpoch, i_CardsInDeck, i_NumEpochs));
}

void AOREventGenerator::UnicastSHIP(const std::string &i_PlayerName, const std::string &i_Capital, const int &i_ShipType, const int &i_ShipNum) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeSHIPMessage(i_Capital, i_ShipType, i_ShipNum));
}
void AOREventGenerator::BroadcastSHIP(const std::string &i_Capital, const int &i_ShipType, const int &i_ShipNum) const
{
  m_OutputPort.BroadCast(MakeSHIPMessage(i_Capital, i_ShipType, i_ShipNum));
}
void AOREventGenerator::VaricastSHIP(const NameBoolean &i_Variator, const std::string &i_Capital, const int &i_ShipType, const int &i_ShipNum) const
{
  m_OutputPort.VariCast(i_Variator,MakeSHIPMessage(i_Capital, i_ShipType, i_ShipNum));
}

void AOREventGenerator::UnicastLEGALACTION(const std::string &i_PlayerName, const std::string &i_ActionName) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeLEGALACTIONMessage(i_ActionName));
}
void AOREventGenerator::BroadcastLEGALACTION(const std::string &i_ActionName) const
{
  m_OutputPort.BroadCast(MakeLEGALACTIONMessage(i_ActionName));
}
void AOREventGenerator::VaricastLEGALACTION(const NameBoolean &i_Variator, const std::string &i_ActionName) const
{
  m_OutputPort.VariCast(i_Variator,MakeLEGALACTIONMessage(i_ActionName));
}

void AOREventGenerator::UnicastMESSAGE(const std::string &i_PlayerName, const std::string &i_MessageText) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeMESSAGEMessage(i_MessageText));
}
void AOREventGenerator::BroadcastMESSAGE(const std::string &i_MessageText) const
{
  m_OutputPort.BroadCast(MakeMESSAGEMessage(i_MessageText));
}
void AOREventGenerator::VaricastMESSAGE(const NameBoolean &i_Variator, const std::string &i_MessageText) const
{
  m_OutputPort.VariCast(i_Variator,MakeMESSAGEMessage(i_MessageText));
}

void AOREventGenerator::UnicastNUMSTOCK(const std::string &i_PlayerName, const int &i_PlayerNum, const int &i_DominationsInStock, const int &i_DominationsOnBoard, const int &i_TokensInStock, const int &i_TokensOnBoard, const int &i_ExpansionTokens) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeNUMSTOCKMessage(i_PlayerNum, i_DominationsInStock, i_DominationsOnBoard, i_TokensInStock, i_TokensOnBoard, i_ExpansionTokens));
}
void AOREventGenerator::BroadcastNUMSTOCK(const int &i_PlayerNum, const int &i_DominationsInStock, const int &i_DominationsOnBoard, const int &i_TokensInStock, const int &i_TokensOnBoard, const int &i_ExpansionTokens) const
{
  m_OutputPort.BroadCast(MakeNUMSTOCKMessage(i_PlayerNum, i_DominationsInStock, i_DominationsOnBoard, i_TokensInStock, i_TokensOnBoard, i_ExpansionTokens));
}
void AOREventGenerator::VaricastNUMSTOCK(const NameBoolean &i_Variator, const int &i_PlayerNum, const int &i_DominationsInStock, const int &i_DominationsOnBoard, const int &i_TokensInStock, const int &i_TokensOnBoard, const int &i_ExpansionTokens) const
{
  m_OutputPort.VariCast(i_Variator,MakeNUMSTOCKMessage(i_PlayerNum, i_DominationsInStock, i_DominationsOnBoard, i_TokensInStock, i_TokensOnBoard, i_ExpansionTokens));
}

void AOREventGenerator::UnicastSURPLUSSHORTAGE(const std::string &i_PlayerName, const std::string &i_Commodity, const std::string &i_State) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeSURPLUSSHORTAGEMessage(i_Commodity, i_State));
}
void AOREventGenerator::BroadcastSURPLUSSHORTAGE(const std::string &i_Commodity, const std::string &i_State) const
{
  m_OutputPort.BroadCast(MakeSURPLUSSHORTAGEMessage(i_Commodity, i_State));
}
void AOREventGenerator::VaricastSURPLUSSHORTAGE(const NameBoolean &i_Variator, const std::string &i_Commodity, const std::string &i_State) const
{
  m_OutputPort.VariCast(i_Variator,MakeSURPLUSSHORTAGEMessage(i_Commodity, i_State));
}

void AOREventGenerator::UnicastSPACEEMPTY(const std::string &i_PlayerName, const std::string &i_SpaceName) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeSPACEEMPTYMessage(i_SpaceName));
}
void AOREventGenerator::BroadcastSPACEEMPTY(const std::string &i_SpaceName) const
{
  m_OutputPort.BroadCast(MakeSPACEEMPTYMessage(i_SpaceName));
}
void AOREventGenerator::VaricastSPACEEMPTY(const NameBoolean &i_Variator, const std::string &i_SpaceName) const
{
  m_OutputPort.VariCast(i_Variator,MakeSPACEEMPTYMessage(i_SpaceName));
}

void AOREventGenerator::UnicastDOMINANCE(const std::string &i_PlayerName, const std::string &i_SpaceName, const std::string &i_OwnerCapital, const std::string &i_Age) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeDOMINANCEMessage(i_SpaceName, i_OwnerCapital, i_Age));
}
void AOREventGenerator::BroadcastDOMINANCE(const std::string &i_SpaceName, const std::string &i_OwnerCapital, const std::string &i_Age) const
{
  m_OutputPort.BroadCast(MakeDOMINANCEMessage(i_SpaceName, i_OwnerCapital, i_Age));
}
void AOREventGenerator::VaricastDOMINANCE(const NameBoolean &i_Variator, const std::string &i_SpaceName, const std::string &i_OwnerCapital, const std::string &i_Age) const
{
  m_OutputPort.VariCast(i_Variator,MakeDOMINANCEMessage(i_SpaceName, i_OwnerCapital, i_Age));
}

void AOREventGenerator::UnicastCAPITALBID(const std::string &i_PlayerName, const int &i_PlayerNum, const int &i_Bid) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeCAPITALBIDMessage(i_PlayerNum, i_Bid));
}
void AOREventGenerator::BroadcastCAPITALBID(const int &i_PlayerNum, const int &i_Bid) const
{
  m_OutputPort.BroadCast(MakeCAPITALBIDMessage(i_PlayerNum, i_Bid));
}
void AOREventGenerator::VaricastCAPITALBID(const NameBoolean &i_Variator, const int &i_PlayerNum, const int &i_Bid) const
{
  m_OutputPort.VariCast(i_Variator,MakeCAPITALBIDMessage(i_PlayerNum, i_Bid));
}

void AOREventGenerator::UnicastRESET(const std::string &i_PlayerName) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeRESETMessage());
}
void AOREventGenerator::BroadcastRESET() const
{
  m_OutputPort.BroadCast(MakeRESETMessage());
}
void AOREventGenerator::VaricastRESET(const NameBoolean &i_Variator) const
{
  m_OutputPort.VariCast(i_Variator,MakeRESETMessage());
}

void AOREventGenerator::UnicastCAPITALBIDS(const std::string &i_PlayerName, const std::string &i_CapitalBids) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeCAPITALBIDSMessage(i_CapitalBids));
}
void AOREventGenerator::BroadcastCAPITALBIDS(const std::string &i_CapitalBids) const
{
  m_OutputPort.BroadCast(MakeCAPITALBIDSMessage(i_CapitalBids));
}
void AOREventGenerator::VaricastCAPITALBIDS(const NameBoolean &i_Variator, const std::string &i_CapitalBids) const
{
  m_OutputPort.VariCast(i_Variator,MakeCAPITALBIDSMessage(i_CapitalBids));
}

void AOREventGenerator::UnicastTOKEN(const std::string &i_PlayerName, const std::string &i_SpaceName, const int &i_TokenNum, const std::string &i_OwnerCapital, const std::string &i_Age) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeTOKENMessage(i_SpaceName, i_TokenNum, i_OwnerCapital, i_Age));
}
void AOREventGenerator::BroadcastTOKEN(const std::string &i_SpaceName, const int &i_TokenNum, const std::string &i_OwnerCapital, const std::string &i_Age) const
{
  m_OutputPort.BroadCast(MakeTOKENMessage(i_SpaceName, i_TokenNum, i_OwnerCapital, i_Age));
}
void AOREventGenerator::VaricastTOKEN(const NameBoolean &i_Variator, const std::string &i_SpaceName, const int &i_TokenNum, const std::string &i_OwnerCapital, const std::string &i_Age) const
{
  m_OutputPort.VariCast(i_Variator,MakeTOKENMessage(i_SpaceName, i_TokenNum, i_OwnerCapital, i_Age));
}

void AOREventGenerator::UnicastNEWSTATE(const std::string &i_PlayerName, const std::string &i_EventName) const
{
  m_OutputPort.UniCast(i_PlayerName,MakeNEWSTATEMessage(i_EventName));
}
void AOREventGenerator::BroadcastNEWSTATE(const std::string &i_EventName) const
{
  m_OutputPort.BroadCast(MakeNEWSTATEMessage(i_EventName));
}
void AOREventGenerator::VaricastNEWSTATE(const NameBoolean &i_Variator, const std::string &i_EventName) const
{
  m_OutputPort.VariCast(i_Variator,MakeNEWSTATEMessage(i_EventName));
}

std::string AOREventGenerator::MakeMISERYMessage(const std::string &i_Capital, const int &i_MiseryIndex) const
{
  std::ostringstream oss;
  oss << "MISERY"
  << ',' << UnCommaStringify(i_Capital) << ',' << UnCommaStringify(i_MiseryIndex); 
  return oss.str();
}

std::string AOREventGenerator::MakeHANDCARDMessage(const std::string &i_CardName, const int &i_Num) const
{
  std::ostringstream oss;
  oss << "HANDCARD"
  << ',' << UnCommaStringify(i_CardName) << ',' << UnCommaStringify(i_Num); 
  return oss.str();
}

std::string AOREventGenerator::MakePLAYERMessage(const std::string &i_PName, const int &i_PlayerNum) const
{
  std::ostringstream oss;
  oss << "PLAYER"
  << ',' << UnCommaStringify(i_PName) << ',' << UnCommaStringify(i_PlayerNum); 
  return oss.str();
}

std::string AOREventGenerator::MakeADVANCESTATUSMessage(const std::string &i_Capital, const char &i_Advance, const std::string &i_Status, const int &i_Cost) const
{
  std::ostringstream oss;
  oss << "ADVANCESTATUS"
  << ',' << UnCommaStringify(i_Capital) << ',' << UnCommaStringify(i_Advance) << ',' << UnCommaStringify(i_Status) << ',' << UnCommaStringify(i_Cost); 
  return oss.str();
}

std::string AOREventGenerator::MakePLAYERCAPITALMessage(const int &i_PlayerNum, const std::string &i_CapitalName) const
{
  std::ostringstream oss;
  oss << "PLAYERCAPITAL"
  << ',' << UnCommaStringify(i_PlayerNum) << ',' << UnCommaStringify(i_CapitalName); 
  return oss.str();
}

std::string AOREventGenerator::MakeCOMMODITYNUMMessage(const std::string &i_Commodity, const std::string &i_OwnerCapital, const int &i_NumSpaces, const int &i_NumNewSpaces) const
{
  std::ostringstream oss;
  oss << "COMMODITYNUM"
  << ',' << UnCommaStringify(i_Commodity) << ',' << UnCommaStringify(i_OwnerCapital) << ',' << UnCommaStringify(i_NumSpaces) << ',' << UnCommaStringify(i_NumNewSpaces); 
  return oss.str();
}

std::string AOREventGenerator::MakeCASHMessage(const int &i_PlayerNum, const int &i_CurCash) const
{
  std::ostringstream oss;
  oss << "CASH"
  << ',' << UnCommaStringify(i_PlayerNum) << ',' << UnCommaStringify(i_CurCash); 
  return oss.str();
}

std::string AOREventGenerator::MakeTURNLOGMessage(const int &i_PlayerNum, const int &i_Total, const int &i_Turn, const int &i_Tokens, const int &i_Cash) const
{
  std::ostringstream oss;
  oss << "TURNLOG"
  << ',' << UnCommaStringify(i_PlayerNum) << ',' << UnCommaStringify(i_Total) << ',' << UnCommaStringify(i_Turn) << ',' << UnCommaStringify(i_Tokens) << ',' << UnCommaStringify(i_Cash); 
  return oss.str();
}

std::string AOREventGenerator::MakeNUMCARDSMessage(const int &i_PlayerNum, const int &i_NumCards) const
{
  std::ostringstream oss;
  oss << "NUMCARDS"
  << ',' << UnCommaStringify(i_PlayerNum) << ',' << UnCommaStringify(i_NumCards); 
  return oss.str();
}

std::string AOREventGenerator::MakeCAPITALSMessage(const std::string &i_Capitals) const
{
  std::ostringstream oss;
  oss << "CAPITALS"
  << ',' << UnCommaStringify(i_Capitals); 
  return oss.str();
}

std::string AOREventGenerator::MakeERRORMessage(const std::string &i_ErrorText) const
{
  std::ostringstream oss;
  oss << "ERROR"
  << ',' << UnCommaStringify(i_ErrorText); 
  return oss.str();
}

std::string AOREventGenerator::MakeEPOCHSTATUSMessage(const int &i_TurnNum, const int &i_CurEpoch, const int &i_CardsInDeck, const int &i_NumEpochs) const
{
  std::ostringstream oss;
  oss << "EPOCHSTATUS"
  << ',' << UnCommaStringify(i_TurnNum) << ',' << UnCommaStringify(i_CurEpoch) << ',' << UnCommaStringify(i_CardsInDeck) << ',' << UnCommaStringify(i_NumEpochs); 
  return oss.str();
}

std::string AOREventGenerator::MakeSHIPMessage(const std::string &i_Capital, const int &i_ShipType, const int &i_ShipNum) const
{
  std::ostringstream oss;
  oss << "SHIP"
  << ',' << UnCommaStringify(i_Capital) << ',' << UnCommaStringify(i_ShipType) << ',' << UnCommaStringify(i_ShipNum); 
  return oss.str();
}

std::string AOREventGenerator::MakeLEGALACTIONMessage(const std::string &i_ActionName) const
{
  std::ostringstream oss;
  oss << "LEGALACTION"
  << ',' << UnCommaStringify(i_ActionName); 
  return oss.str();
}

std::string AOREventGenerator::MakeMESSAGEMessage(const std::string &i_MessageText) const
{
  std::ostringstream oss;
  oss << "MESSAGE"
  << ',' << UnCommaStringify(i_MessageText); 
  return oss.str();
}

std::string AOREventGenerator::MakeNUMSTOCKMessage(const int &i_PlayerNum, const int &i_DominationsInStock, const int &i_DominationsOnBoard, const int &i_TokensInStock, const int &i_TokensOnBoard, const int &i_ExpansionTokens) const
{
  std::ostringstream oss;
  oss << "NUMSTOCK"
  << ',' << UnCommaStringify(i_PlayerNum) << ',' << UnCommaStringify(i_DominationsInStock) << ',' << UnCommaStringify(i_DominationsOnBoard) << ',' << UnCommaStringify(i_TokensInStock) << ',' << UnCommaStringify(i_TokensOnBoard) << ',' << UnCommaStringify(i_ExpansionTokens); 
  return oss.str();
}

std::string AOREventGenerator::MakeSURPLUSSHORTAGEMessage(const std::string &i_Commodity, const std::string &i_State) const
{
  std::ostringstream oss;
  oss << "SURPLUSSHORTAGE"
  << ',' << UnCommaStringify(i_Commodity) << ',' << UnCommaStringify(i_State); 
  return oss.str();
}

std::string AOREventGenerator::MakeSPACEEMPTYMessage(const std::string &i_SpaceName) const
{
  std::ostringstream oss;
  oss << "SPACEEMPTY"
  << ',' << UnCommaStringify(i_SpaceName); 
  return oss.str();
}

std::string AOREventGenerator::MakeDOMINANCEMessage(const std::string &i_SpaceName, const std::string &i_OwnerCapital, const std::string &i_Age) const
{
  std::ostringstream oss;
  oss << "DOMINANCE"
  << ',' << UnCommaStringify(i_SpaceName) << ',' << UnCommaStringify(i_OwnerCapital) << ',' << UnCommaStringify(i_Age); 
  return oss.str();
}

std::string AOREventGenerator::MakeCAPITALBIDMessage(const int &i_PlayerNum, const int &i_Bid) const
{
  std::ostringstream oss;
  oss << "CAPITALBID"
  << ',' << UnCommaStringify(i_PlayerNum) << ',' << UnCommaStringify(i_Bid); 
  return oss.str();
}

std::string AOREventGenerator::MakeRESETMessage() const
{
  std::ostringstream oss;
  oss << "RESET"
  << ',' << m_ServerGameInfo.GetName();
  return oss.str();
}

std::string AOREventGenerator::MakeCAPITALBIDSMessage(const std::string &i_CapitalBids) const
{
  std::ostringstream oss;
  oss << "CAPITALBIDS"
  << ',' << UnCommaStringify(i_CapitalBids); 
  return oss.str();
}

std::string AOREventGenerator::MakeTOKENMessage(const std::string &i_SpaceName, const int &i_TokenNum, const std::string &i_OwnerCapital, const std::string &i_Age) const
{
  std::ostringstream oss;
  oss << "TOKEN"
  << ',' << UnCommaStringify(i_SpaceName) << ',' << UnCommaStringify(i_TokenNum) << ',' << UnCommaStringify(i_OwnerCapital) << ',' << UnCommaStringify(i_Age); 
  return oss.str();
}

std::string AOREventGenerator::MakeNEWSTATEMessage(const std::string &i_EventName) const
{
  std::ostringstream oss;
  oss << "NEWSTATE"
  << ',' << UnCommaStringify(i_EventName); 
  return oss.str();
}


class AORGameInfo : public GameInfo<AORSet,AORState,AOREventGenerator>
{
public:
  AORGameInfo(const AORSet &i_Set,
                  const ServerGameInfo &i_ginfo,
                  OutputPort  &i_Port) :
    GameInfo<AORSet,AORState,AOREventGenerator>(i_Set,i_ginfo,i_Port) {}

private:
  static const StateName OrderOfPlayState;
  static const StateName PlaceBidsState;
  static const StateName ChooseCapitalState;
  static const StateName InitialState;
  static const StateName OrderOfPlayStartState;
  static const StateName TerminalState;
public:
  bool MAKECAPITALBID_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
  {
    return 
      GetPlayers().IsPlayer(i_PlayerName)
    ;
  }

  bool MAKECAPITALBID_IsAuto(const StateName &i_StateName) const
  {
    return false;
  }

  bool MAKECAPITALBID_ExecuteAction(const std::string &i_PlayerName,const ActionParser &i_ap,
                              const StateName &i_StateName)
  {
    if (i_ap.GetNumArguments() != 1)
    {
      UnicastERROR(i_PlayerName,"Bad number of arguments to MAKECAPITALBID");
      return false;
    }

    int capitalbid(atoi(i_ap[0].c_str()));

    if (capitalbid > INITIALSTAKE)
    {
      UnicastERROR(i_PlayerName,"Action MAKECAPITALBID variable capitalbid is above maximum");
      return false;
    }

    
      
      size_t playerId = GetPlayers().GetPlayerId(i_PlayerName);
      Player &curPlayer=GetPlayers()[playerId];
      curPlayer.SetCapitalBid(capitalbid);
      curPlayer.SetCash(INITIALSTAKE - capitalbid);
      
      BroadcastCAPITALBID(playerId,SECRET_BID);
      UnicastCAPITALBID(i_PlayerName,playerId,capitalbid);
      UnicastCASH(i_PlayerName,playerId,curPlayer.GetCash());

      return true;
      
    ;
  }

  bool JOIN_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
  {
    return  
      
      GetPlayers().size() < 6 && !GetPlayers().IsPlayer(i_PlayerName)
      
    ;
  }

  bool JOIN_IsAuto(const StateName &i_StateName) const
  {
    return false;
  }

  bool JOIN_ExecuteAction(const std::string &i_PlayerName,const ActionParser &i_ap,
                              const StateName &i_StateName)
  {
    if (i_ap.GetNumArguments() != 0)
    {
      UnicastERROR(i_PlayerName,"Bad number of arguments to JOIN");
      return false;
    }

    
      GetPlayers().add(i_PlayerName);
      BroadcastPLAYER(i_PlayerName,GetPlayers().GetPlayerId(i_PlayerName));
      return true;
    ;
  }

  bool INITORDEROFPLAY_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
  {
    return true;
  }

  bool INITORDEROFPLAY_IsAuto(const StateName &i_StateName) const
  {
    return true;
  }

  bool INITORDEROFPLAY_ExecuteAction(const std::string &i_PlayerName,const ActionParser &i_ap,
                              const StateName &i_StateName)
  {
    if (i_ap.GetNumArguments() != 0)
    {
      UnicastERROR(i_PlayerName,"Bad number of arguments to INITORDEROFPLAY");
      return false;
    }

    
      
      GetPlayers().ExtendTurnLog(GetCurTurn() + 1);
      
      size_t i;
      for (i = 0 ; i < GetPlayers().size() ; ++i)
      {
        GetPlayers()[i].ClearExpansionTokens();
	BroadcastTURNLOG(i,SECRET_BID,GetCurTurn()+1,NO_BID,NO_BID);
	UnicastTURNLOG(GetPlayers()[i].GetName(),
	               i,
		       GetPlayers()[i].GetCash(),
		       GetCurTurn()+1,
		       NO_BID,
		       NO_BID);
      }	

      return true;
      
    ;
  }

  bool LASTCAPITAL_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
  {
    return true;
  }

  bool LASTCAPITAL_IsAuto(const StateName &i_StateName) const
  {
    return  
      GetCapitalChooser() == GetPlayers().size() - 1
    ;
  }

  bool LASTCAPITAL_ExecuteAction(const std::string &i_PlayerName,const ActionParser &i_ap,
                              const StateName &i_StateName)
  {
    if (i_ap.GetNumArguments() != 0)
    {
      UnicastERROR(i_PlayerName,"Bad number of arguments to LASTCAPITAL");
      return false;
    }

    
    
      // well, if we're here, we're all legal.
      size_t playerId = GetCapitalChooser();
      Player &rPlayer = GetPlayers()[playerId];
      std::string capital = *(GetCapitalChoiceSet().begin());

      rPlayer.SetCapital(capital);
      BroadcastPLAYERCAPITAL(playerId,capital);

      // this changes the capital choice list and CapitalChooser
      ClearCapitalChoice(capital);
      BroadcastCAPITALS(GetCapitalChoiceList());

      BroadcastSHIP(capital,rPlayer.GetShipType(),rPlayer.GetShipGrade());
      BroadcastMISERY(capital,rPlayer.GetMiseryIndex());

      // place their home Dom Marker on the board.
      rPlayer.DecrementDominations();
      BroadcastNUMSTOCK(playerId,
		        rPlayer.GetNumDominations(),
			rPlayer.GetNumBoardDominations(),
			rPlayer.GetNumStockTokens(),
			rPlayer.GetNumBoardTokens(),
			rPlayer.GetNumExpansionTokens());

      GetBoardState().SetDominance(capital,capital,true);
      GetBoardState().BroadcastSpaceInfo(*this,capital);
      GetBoardState().BroadcastCommodityLogInfo(*this,capital,GetPlayers(),GetCapitalChooser());
      
      return true;
    
    ;
  }

  bool STARTGAME_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
  {
    return 
      
        GetPlayers().size() >= 3 && GetPlayers().IsPlayer(i_PlayerName)
      
    ;
  }

  bool STARTGAME_IsAuto(const StateName &i_StateName) const
  {
    return false;
  }

  bool STARTGAME_ExecuteAction(const std::string &i_PlayerName,const ActionParser &i_ap,
                              const StateName &i_StateName)
  {
    if (i_ap.GetNumArguments() != 1)
    {
      UnicastERROR(i_PlayerName,"Bad number of arguments to STARTGAME");
      return false;
    }

    int numepochs(atoi(i_ap[0].c_str()));

    if (numepochs < 1)
    {
      UnicastERROR(i_PlayerName,"Action STARTGAME variable numepochs is below minimum");
      return false;
    }

    if (numepochs > 3)
    {
      UnicastERROR(i_PlayerName,"Action STARTGAME variable numepochs is above maximum");
      return false;
    }

    
      
      GetCardManager().SetNumEpochs(numepochs);

      size_t i;
      for (i = 0 ; i < GetPlayers().size() ; ++ i)
      {  
         Player &curPlayer=GetPlayers()[i];   
         std::string cardName = GetCardManager().DrawCard();
	 curPlayer.AddCard(cardName);
	 BroadcastNUMCARDS(i,1);
	 UnicastHANDCARD(curPlayer.GetName(),cardName,0);
	 curPlayer.SetCash(INITIALSTAKE);
	 UnicastCASH(curPlayer.GetName(),i,INITIALSTAKE);
	 curPlayer.SetCapitalBid(NO_BID);
	 BroadcastCAPITALBID(i,NO_BID);
      }      

      BroadcastEPOCHSTATUS(0,1,GetCardManager().GetNumCards(),numepochs);
      BroadcastCAPITALBIDS(GetLegalBidString(0,INITIALSTAKE));
      return true;
      
     ;
  }

  bool BIDSDONE_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
  {
    return true;
  }

  bool BIDSDONE_IsAuto(const StateName &i_StateName) const
  {
    return 
      GetPlayers().AllBid()
    ;
  }

  bool BIDSDONE_ExecuteAction(const std::string &i_PlayerName,const ActionParser &i_ap,
                              const StateName &i_StateName)
  {
    if (i_ap.GetNumArguments() != 0)
    {
      UnicastERROR(i_PlayerName,"Bad number of arguments to BIDSDONE");
      return false;
    }

     
      
      GetPlayers().ExecuteCapitalBid();
      PrepareCapitalChoices();
      BroadcastCAPITALS(GetCapitalChoiceList());      


      for (size_t i = 0 ; i < GetPlayers().size() ; ++i)
      {     
        Player &curPlayer = GetPlayers()[i];
	SendFullState(curPlayer.GetName(),i_StateName);
      }        

      return true;
      
    ;
  }

  bool CHOOSECAPITAL_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
  {
    return  GetCapitalChooser() == GetPlayers().GetPlayerId(i_PlayerName) ;
  }

  bool CHOOSECAPITAL_IsAuto(const StateName &i_StateName) const
  {
    return false;
  }

  bool CHOOSECAPITAL_ExecuteAction(const std::string &i_PlayerName,const ActionParser &i_ap,
                              const StateName &i_StateName)
  {
    if (i_ap.GetNumArguments() != 1)
    {
      UnicastERROR(i_PlayerName,"Bad number of arguments to CHOOSECAPITAL");
      return false;
    }

    std::string capital((i_ap[0].c_str()));

    const std::set<std::string> &validlist(GetCapitalChoiceSet());
    std::set<std::string>::const_iterator findit = validlist.find(capital);
    if (findit == validlist.end())
    {
      UnicastERROR(i_PlayerName,"Action CHOOSECAPITAL variable capital has illegal value");
      return false;
    }

    
    
      // well, if we're here, we're all legal.
      size_t playerId = GetCapitalChooser();
      Player &rPlayer = GetPlayers()[playerId];
      rPlayer.SetCapital(capital);
      BroadcastPLAYERCAPITAL(playerId,capital);

      // this changes the capital choice list and CapitalChooser
      ClearCapitalChoice(capital);
      BroadcastCAPITALS(GetCapitalChoiceList());

      BroadcastSHIP(capital,rPlayer.GetShipType(),rPlayer.GetShipGrade());
      BroadcastMISERY(capital,rPlayer.GetMiseryIndex());

      // place their home Dom Marker on the board.
      rPlayer.DecrementDominations();
      BroadcastNUMSTOCK(playerId,
		        rPlayer.GetNumDominations(),
			rPlayer.GetNumBoardDominations(),
			rPlayer.GetNumStockTokens(),
			rPlayer.GetNumBoardTokens(),
			rPlayer.GetNumExpansionTokens());

      GetBoardState().SetDominance(capital,capital,true);
      GetBoardState().BroadcastSpaceInfo(*this,capital);
      GetBoardState().BroadcastCommodityLogInfo(*this,capital,GetPlayers(),GetCapitalChooser());

      return true;
    
    ;
  }

  bool MAKEORDEROFPLAYBID_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const
  {
    return 
      GetPlayers().IsPlayer(i_PlayerName)
    ;
  }

  bool MAKEORDEROFPLAYBID_IsAuto(const StateName &i_StateName) const
  {
    return false;
  }

  bool MAKEORDEROFPLAYBID_ExecuteAction(const std::string &i_PlayerName,const ActionParser &i_ap,
                              const StateName &i_StateName)
  {
    if (i_ap.GetNumArguments() != 1)
    {
      UnicastERROR(i_PlayerName,"Bad number of arguments to MAKEORDEROFPLAYBID");
      return false;
    }

    int oopbid(atoi(i_ap[0].c_str()));

    if (oopbid < -GetPlayers()[GetPlayers().GetPlayerId(i_PlayerName)].GetCash())
    {
      UnicastERROR(i_PlayerName,"Action MAKEORDEROFPLAYBID variable oopbid is below minimum");
      return false;
    }

    if (oopbid > GetPlayers()[GetPlayers().GetPlayerId(i_PlayerName)].GetCash())
    {
      UnicastERROR(i_PlayerName,"Action MAKEORDEROFPLAYBID variable oopbid is above maximum");
      return false;
    }

    
      
      size_t playerId = GetPlayers().GetPlayerId(i_PlayerName);
      Player &curPlayer=GetPlayers()[playerId];
      TurnLogEntry &curentry = curPlayer.GetLastTurnLogEntry();
      curentry.SetTokens(oopbid);

      BroadcastTURNLOG(playerId,SECRET_BID,GetCurTurn()+1,SECRET_BID,SECRET_BID);
      UnicastTURNLOG(i_PlayerName,
                     playerId,
		     curentry.GetTotal(),
		     GetCurTurn() + 1,
		     curentry.GetTokens(),
		     curentry.GetCash());
          



      return true;
      
    ;
  }


  void SendFullState(const std::string &i_PlayerName,const StateName &i_StateName) const
  {

    
    UnicastRESET(i_PlayerName);
    UnicastCAPITALBIDS(i_PlayerName,GetLegalBidString(0,INITIALSTAKE));
    size_t i;
    for (i = 0 ; i < GetPlayers().size() ; ++i)
    {
        UnicastPLAYER(i_PlayerName,GetPlayers()[i].GetName(),i);
    }

    // --------- INITIAL state only gets names.
    if (i_StateName == InitialState) return;

    UnicastEPOCHSTATUS(i_PlayerName,
                       GetCurTurn(),
                       GetCardManager().GetCurEpoch(),
		       GetCardManager().GetNumCards(),
	               GetCardManager().GetNumEpochs());

    for (i = 0 ; i < GetPlayers().size() ; ++i)
    {
        UnicastNUMCARDS(i_PlayerName,i,GetPlayers()[i].GetNumCards());
    }

    size_t playerId = GetPlayers().GetPlayerId(i_PlayerName);
    if (playerId != NO_PLAYER)
    {
       const Player &curPlayer = GetPlayers()[playerId];
       const std::vector<std::string> &cvec = curPlayer.GetHand();
       for (i = 0 ; i < cvec.size() ; ++i)
       {
         UnicastHANDCARD(i_PlayerName,cvec[i],i);
       }
       UnicastCASH(i_PlayerName,playerId,curPlayer.GetCash());
    }

    for (i = 0 ; i < GetPlayers().size() ; ++i)
    {
      int cb = GetPlayers()[i].GetCapitalBid();
      if (cb == -1)
      {
        UnicastCAPITALBID(i_PlayerName,i,NO_BID);
      }
      else if (i_StateName == PlaceBidsState && i != playerId)
      {
        UnicastCAPITALBID(i_PlayerName,i,SECRET_BID);
      }
      else
      {
        UnicastCAPITALBID(i_PlayerName,i,cb);
      }
    }
    
    // this is all we do if not all players have made bid yet.
    if (i_StateName == PlaceBidsState) return;

    UnicastCAPITALS(i_PlayerName,GetCapitalChoiceList());
    for (i = 0 ; i < GetCapitalChooser() ; ++i )
    {
      const Player &rPlayer = GetPlayers()[i];
      UnicastPLAYERCAPITAL(i_PlayerName,i,rPlayer.GetCapital());
      UnicastSHIP(i_PlayerName,rPlayer.GetCapital(),
                  rPlayer.GetShipType(),rPlayer.GetShipGrade());
      UnicastMISERY(i_PlayerName,rPlayer.GetCapital(),rPlayer.GetMiseryIndex());

      UnicastNUMSTOCK(i_PlayerName,i,
		      rPlayer.GetNumDominations(),
		      rPlayer.GetNumBoardDominations(),
		      rPlayer.GetNumStockTokens(),
		      rPlayer.GetNumBoardTokens(),
		      rPlayer.GetNumExpansionTokens());

      const std::vector<TurnLogEntry> &turnlog = rPlayer.GetTurnLog();
      for (size_t j = 0 ; j < turnlog.size() ; ++j)
      {
        int total;
	int tokens;
	int cash;

	turnlog[j].GetObservedInfo(i==playerId,total,tokens,cash);
        UnicastTURNLOG(i_PlayerName,
	               i,
	               total,
		       turnlog[j].GetTurn(),
		       tokens,
		       cash);
      }

    }

    // board
    GetBoardState().SendAllSpaceInfo(*this,i_PlayerName);
    GetBoardState().SendAllCommodityLogInfo(*this,i_PlayerName,GetPlayers(),
                                            GetCapitalChooser(),
                                            GetAORSet().GetCommodityInfo());

    

    
  
  }
};

const StateName AORGameInfo::OrderOfPlayState = "OrderOfPlayState";
const StateName AORGameInfo::PlaceBidsState = "PlaceBidsState";
const StateName AORGameInfo::ChooseCapitalState = "ChooseCapitalState";
const StateName AORGameInfo::InitialState = "InitialState";
const StateName AORGameInfo::OrderOfPlayStartState = "OrderOfPlayStartState";
const StateName AORGameInfo::TerminalState = "TerminalState";
typedef DLLGame<AORSet,AORGameInfo> AORGame;

class MAKECAPITALBID_Transition : public Transition<AORGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,
                       const StateName &i_StateName,const AORGameInfo &i_gi) const
  {
    return i_gi.MAKECAPITALBID_IsLegal(i_PlayerName,i_StateName);
  }

  virtual bool IsAuto(const StateName &i_StateName,const AORGameInfo &i_gi) const
  {
    return i_gi.MAKECAPITALBID_IsAuto(i_StateName);
  }

  virtual bool ExecuteAction(const StateName &i_StateName,AORGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const
  {
    return i_gi.MAKECAPITALBID_ExecuteAction(i_PlayerName,i_ap,i_StateName);
  }

  virtual std::string GetName() const
  {
    return "MAKECAPITALBID";
  }
};

class JOIN_Transition : public Transition<AORGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,
                       const StateName &i_StateName,const AORGameInfo &i_gi) const
  {
    return i_gi.JOIN_IsLegal(i_PlayerName,i_StateName);
  }

  virtual bool IsAuto(const StateName &i_StateName,const AORGameInfo &i_gi) const
  {
    return i_gi.JOIN_IsAuto(i_StateName);
  }

  virtual bool ExecuteAction(const StateName &i_StateName,AORGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const
  {
    return i_gi.JOIN_ExecuteAction(i_PlayerName,i_ap,i_StateName);
  }

  virtual std::string GetName() const
  {
    return "JOIN";
  }
};

class INITORDEROFPLAY_Transition : public Transition<AORGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,
                       const StateName &i_StateName,const AORGameInfo &i_gi) const
  {
    return i_gi.INITORDEROFPLAY_IsLegal(i_PlayerName,i_StateName);
  }

  virtual bool IsAuto(const StateName &i_StateName,const AORGameInfo &i_gi) const
  {
    return i_gi.INITORDEROFPLAY_IsAuto(i_StateName);
  }

  virtual bool ExecuteAction(const StateName &i_StateName,AORGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const
  {
    return i_gi.INITORDEROFPLAY_ExecuteAction(i_PlayerName,i_ap,i_StateName);
  }

  virtual std::string GetName() const
  {
    return "INITORDEROFPLAY";
  }
};

class LASTCAPITAL_Transition : public Transition<AORGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,
                       const StateName &i_StateName,const AORGameInfo &i_gi) const
  {
    return i_gi.LASTCAPITAL_IsLegal(i_PlayerName,i_StateName);
  }

  virtual bool IsAuto(const StateName &i_StateName,const AORGameInfo &i_gi) const
  {
    return i_gi.LASTCAPITAL_IsAuto(i_StateName);
  }

  virtual bool ExecuteAction(const StateName &i_StateName,AORGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const
  {
    return i_gi.LASTCAPITAL_ExecuteAction(i_PlayerName,i_ap,i_StateName);
  }

  virtual std::string GetName() const
  {
    return "LASTCAPITAL";
  }
};

class STARTGAME_Transition : public Transition<AORGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,
                       const StateName &i_StateName,const AORGameInfo &i_gi) const
  {
    return i_gi.STARTGAME_IsLegal(i_PlayerName,i_StateName);
  }

  virtual bool IsAuto(const StateName &i_StateName,const AORGameInfo &i_gi) const
  {
    return i_gi.STARTGAME_IsAuto(i_StateName);
  }

  virtual bool ExecuteAction(const StateName &i_StateName,AORGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const
  {
    return i_gi.STARTGAME_ExecuteAction(i_PlayerName,i_ap,i_StateName);
  }

  virtual std::string GetName() const
  {
    return "STARTGAME";
  }
};

class BIDSDONE_Transition : public Transition<AORGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,
                       const StateName &i_StateName,const AORGameInfo &i_gi) const
  {
    return i_gi.BIDSDONE_IsLegal(i_PlayerName,i_StateName);
  }

  virtual bool IsAuto(const StateName &i_StateName,const AORGameInfo &i_gi) const
  {
    return i_gi.BIDSDONE_IsAuto(i_StateName);
  }

  virtual bool ExecuteAction(const StateName &i_StateName,AORGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const
  {
    return i_gi.BIDSDONE_ExecuteAction(i_PlayerName,i_ap,i_StateName);
  }

  virtual std::string GetName() const
  {
    return "BIDSDONE";
  }
};

class CHOOSECAPITAL_Transition : public Transition<AORGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,
                       const StateName &i_StateName,const AORGameInfo &i_gi) const
  {
    return i_gi.CHOOSECAPITAL_IsLegal(i_PlayerName,i_StateName);
  }

  virtual bool IsAuto(const StateName &i_StateName,const AORGameInfo &i_gi) const
  {
    return i_gi.CHOOSECAPITAL_IsAuto(i_StateName);
  }

  virtual bool ExecuteAction(const StateName &i_StateName,AORGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const
  {
    return i_gi.CHOOSECAPITAL_ExecuteAction(i_PlayerName,i_ap,i_StateName);
  }

  virtual std::string GetName() const
  {
    return "CHOOSECAPITAL";
  }
};

class MAKEORDEROFPLAYBID_Transition : public Transition<AORGameInfo>
{
public:
  virtual bool IsLegal(const std::string &i_PlayerName,
                       const StateName &i_StateName,const AORGameInfo &i_gi) const
  {
    return i_gi.MAKEORDEROFPLAYBID_IsLegal(i_PlayerName,i_StateName);
  }

  virtual bool IsAuto(const StateName &i_StateName,const AORGameInfo &i_gi) const
  {
    return i_gi.MAKEORDEROFPLAYBID_IsAuto(i_StateName);
  }

  virtual bool ExecuteAction(const StateName &i_StateName,AORGameInfo &i_gi,
                             const std::string &i_PlayerName,
                             const ActionParser &i_ap) const
  {
    return i_gi.MAKEORDEROFPLAYBID_ExecuteAction(i_PlayerName,i_ap,i_StateName);
  }

  virtual std::string GetName() const
  {
    return "MAKEORDEROFPLAYBID";
  }
};

class AORStateMachine : public StateMachine<AORGameInfo>
{
public:
  AORStateMachine()
  {
    StateType *pOrderOfPlayState = new StateType("OrderOfPlayState","Secretly Allocate Cash for Tokens/Turn Order");
    InsertState(pOrderOfPlayState);
    StateType *pPlaceBidsState = new StateType("PlaceBidsState","Game Begins. Place Secret Bids for Capital Choice Order");
    InsertState(pPlaceBidsState);
    StateType *pChooseCapitalState = new StateType("ChooseCapitalState","Choose Capital");
    InsertState(pChooseCapitalState);
    StateType *pInitialState = new StateType("InitialState","Initial Game State");
    InsertState(pInitialState);
    StateType *pOrderOfPlayStartState = new StateType("OrderOfPlayStartState","Initialize Order of Play");
    InsertState(pOrderOfPlayStartState);
    StateType *pTerminalState = new StateType("TerminalState","End Of Game State");
    InsertState(pTerminalState);
    TransitionType *pMAKECAPITALBID_Transition = new MAKECAPITALBID_Transition;
    InsertTransition(pMAKECAPITALBID_Transition);
    TransitionType *pJOIN_Transition = new JOIN_Transition;
    InsertTransition(pJOIN_Transition);
    TransitionType *pINITORDEROFPLAY_Transition = new INITORDEROFPLAY_Transition;
    InsertTransition(pINITORDEROFPLAY_Transition);
    TransitionType *pLASTCAPITAL_Transition = new LASTCAPITAL_Transition;
    InsertTransition(pLASTCAPITAL_Transition);
    TransitionType *pSTARTGAME_Transition = new STARTGAME_Transition;
    InsertTransition(pSTARTGAME_Transition);
    TransitionType *pBIDSDONE_Transition = new BIDSDONE_Transition;
    InsertTransition(pBIDSDONE_Transition);
    TransitionType *pCHOOSECAPITAL_Transition = new CHOOSECAPITAL_Transition;
    InsertTransition(pCHOOSECAPITAL_Transition);
    TransitionType *pMAKEORDEROFPLAYBID_Transition = new MAKEORDEROFPLAYBID_Transition;
    InsertTransition(pMAKEORDEROFPLAYBID_Transition);

    pPlaceBidsState->InsertTransition(*pMAKECAPITALBID_Transition,*pPlaceBidsState);
    pInitialState->InsertTransition(*pJOIN_Transition,*pInitialState);
    pOrderOfPlayStartState->InsertTransition(*pINITORDEROFPLAY_Transition,*pOrderOfPlayState);
    pChooseCapitalState->InsertTransition(*pLASTCAPITAL_Transition,*pOrderOfPlayStartState);
    pInitialState->InsertTransition(*pSTARTGAME_Transition,*pPlaceBidsState);
    pPlaceBidsState->InsertTransition(*pBIDSDONE_Transition,*pChooseCapitalState);
    pChooseCapitalState->InsertTransition(*pCHOOSECAPITAL_Transition,*pChooseCapitalState);
    pOrderOfPlayState->InsertTransition(*pMAKEORDEROFPLAYBID_Transition,*pOrderOfPlayState);
  }
};

AORSet *pGameSet;
AORStateMachine *pStateMachine;

bool Initialize(const std::string &i_DataDir)
{
  pStateMachine = new AORStateMachine;
  pGameSet = new AORSet(i_DataDir);
  if (!pGameSet || !pGameSet->IsValid())
  {
    return false;
  }
  return true;
}

Game *CreateGame(const ServerGameInfo &i_rServerGameInfo,OutputPort &i_rOutputPort)
{
  AORGame *pNewGame = new AORGame(i_rServerGameInfo,i_rOutputPort,
                                   *pGameSet,*pStateMachine,1);
  return pNewGame;
}
