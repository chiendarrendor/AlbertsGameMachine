package Outpost;

import NodeInterfacePackage.*;


public class ServerStateHistory
{
  ServerState m_curstate;
  ServerState m_laststate;
  String m_bidder;
  String m_purchaser;
  String m_iam;

  public ServerState GetLastState() { return m_laststate; }
  public ServerState GetCurState() { return m_curstate; }

  public ServerStateHistory()
  {
    m_curstate = new ServerState(null,null,null);
    m_laststate = new ServerState(null,null,null);
    m_bidder = null;
    m_purchaser = null;
    m_iam = null;
  }

  public void perror(String i_s)
  {
    System.out.println("IAM " + m_iam + ": "+i_s);
  }

  public void HandleEvent(GameEvent i_event)
  {
    if (i_event.GetVarValue("eventname").equals("IAM"))
    {
      m_iam = i_event.GetVarValue("PlayerNumber");
      return;
    }

    if (i_event.GetVarValue("eventname").equals("PLAYERSTATE"))
    {
      if (i_event.GetVarValue("PurchaseState").equals("1"))
      {
        m_purchaser = i_event.GetVarValue("PlayerNumber");
      }

      if (i_event.GetVarValue("BidState").equals("1"))
      {
        m_bidder = i_event.GetVarValue("PlayerNumber");
      }
    }

    if (i_event.GetVarValue("eventname").equals("NEWSTATE"))
    {
      ServerState nextstate;
      String statename = i_event.GetVarValue("statename");

      if (statename.equals("Bid"))
      {
        nextstate = new ServerState(statename,m_purchaser,m_bidder);
      }
      else
      {
        nextstate = new ServerState(statename,m_purchaser,null);
      }
      
      m_laststate = m_curstate;
      m_curstate = nextstate;
    }
  }

  public String toString()
  {
    return("[iam="+m_iam+
           ",laststate="+m_laststate+
           ",curstate="+m_curstate+"]");
  }

}
