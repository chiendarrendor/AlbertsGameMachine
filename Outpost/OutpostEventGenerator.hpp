#ifndef OutpostEVENTGENERATOR
#define OutpostEVENTGENERATOR
#include "OutputPort.hpp"
#include "ServerGameInfo.hpp"
class OutpostEventGenerator
{
public:
  OutpostEventGenerator(OutputPort &i_OutputPort,const ServerGameInfo &i_rSGI);

  void UnicastMEGAREQUEST(const std::string &i_PlayerName, const int &i_resourcetype, const int &i_maxmegas) const;
  void BroadcastMEGAREQUEST(const int &i_resourcetype, const int &i_maxmegas) const;
  void VaricastMEGAREQUEST(const NameBoolean &i_Variator, const int &i_resourcetype, const int &i_maxmegas) const;

  void UnicastPLAYER(const std::string &i_PlayerName, const std::string &i_PName, const int &i_PId) const;
  void BroadcastPLAYER(const std::string &i_PName, const int &i_PId) const;
  void VaricastPLAYER(const NameBoolean &i_Variator, const std::string &i_PName, const int &i_PId) const;

  void UnicastERROR(const std::string &i_PlayerName, const std::string &i_ErrorText) const;
  void BroadcastERROR(const std::string &i_ErrorText) const;
  void VaricastERROR(const NameBoolean &i_Variator, const std::string &i_ErrorText) const;

  void UnicastTURNORDER(const std::string &i_PlayerName, const int &i_playernum, const int &i_turnorder) const;
  void BroadcastTURNORDER(const int &i_playernum, const int &i_turnorder) const;
  void VaricastTURNORDER(const NameBoolean &i_Variator, const int &i_playernum, const int &i_turnorder) const;

  void UnicastSUPPLYSHIP(const std::string &i_PlayerName, const int &i_slotnum, const int &i_contents) const;
  void BroadcastSUPPLYSHIP(const int &i_slotnum, const int &i_contents) const;
  void VaricastSUPPLYSHIP(const NameBoolean &i_Variator, const int &i_slotnum, const int &i_contents) const;

  void UnicastRESET(const std::string &i_PlayerName) const;
  void BroadcastRESET() const;
  void VaricastRESET(const NameBoolean &i_Variator) const;

  void UnicastOPTIONS(const std::string &i_PlayerName, const bool &i_discard, const bool &i_refinery, const bool &i_oneupgrade, const bool &i_activebid, const bool &i_harshsupply, const bool &i_robots, const bool &i_researchmega, const bool &i_illegalbid, const bool &i_smallresearch, const bool &i_smallmicro) const;
  void BroadcastOPTIONS(const bool &i_discard, const bool &i_refinery, const bool &i_oneupgrade, const bool &i_activebid, const bool &i_harshsupply, const bool &i_robots, const bool &i_researchmega, const bool &i_illegalbid, const bool &i_smallresearch, const bool &i_smallmicro) const;
  void VaricastOPTIONS(const NameBoolean &i_Variator, const bool &i_discard, const bool &i_refinery, const bool &i_oneupgrade, const bool &i_activebid, const bool &i_harshsupply, const bool &i_robots, const bool &i_researchmega, const bool &i_illegalbid, const bool &i_smallresearch, const bool &i_smallmicro) const;

  void UnicastLEGALACTION(const std::string &i_PlayerName, const std::string &i_ActionName) const;
  void BroadcastLEGALACTION(const std::string &i_ActionName) const;
  void VaricastLEGALACTION(const NameBoolean &i_Variator, const std::string &i_ActionName) const;

  void UnicastNEWSTATE(const std::string &i_PlayerName, const std::string &i_EventName) const;
  void BroadcastNEWSTATE(const std::string &i_EventName) const;
  void VaricastNEWSTATE(const NameBoolean &i_Variator, const std::string &i_EventName) const;

  void UnicastMESSAGE(const std::string &i_PlayerName, const std::string &i_MessageText) const;
  void BroadcastMESSAGE(const std::string &i_MessageText) const;
  void VaricastMESSAGE(const NameBoolean &i_Variator, const std::string &i_MessageText) const;

  void UnicastITEMSTOCK(const std::string &i_PlayerName, const int &i_stacknum, const int &i_stacksize) const;
  void BroadcastITEMSTOCK(const int &i_stacknum, const int &i_stacksize) const;
  void VaricastITEMSTOCK(const NameBoolean &i_Variator, const int &i_stacknum, const int &i_stacksize) const;

private:
  OutputPort &m_OutputPort;
  const ServerGameInfo &m_ServerGameInfo;
  template<class T_ArgType> std::string UnCommaStringify(const T_ArgType &i_arg) const
  {
    std::ostringstream oss;
    oss << i_arg;
    return UnComma(oss.str());
  }
  std::string MakeMEGAREQUESTMessage(const int &i_resourcetype, const int &i_maxmegas) const;
  std::string MakePLAYERMessage(const std::string &i_PName, const int &i_PId) const;
  std::string MakeERRORMessage(const std::string &i_ErrorText) const;
  std::string MakeTURNORDERMessage(const int &i_playernum, const int &i_turnorder) const;
  std::string MakeSUPPLYSHIPMessage(const int &i_slotnum, const int &i_contents) const;
  std::string MakeRESETMessage() const;
  std::string MakeOPTIONSMessage(const bool &i_discard, const bool &i_refinery, const bool &i_oneupgrade, const bool &i_activebid, const bool &i_harshsupply, const bool &i_robots, const bool &i_researchmega, const bool &i_illegalbid, const bool &i_smallresearch, const bool &i_smallmicro) const;
  std::string MakeLEGALACTIONMessage(const std::string &i_ActionName) const;
  std::string MakeNEWSTATEMessage(const std::string &i_EventName) const;
  std::string MakeMESSAGEMessage(const std::string &i_MessageText) const;
  std::string MakeITEMSTOCKMessage(const int &i_stacknum, const int &i_stacksize) const;
};

#endif
