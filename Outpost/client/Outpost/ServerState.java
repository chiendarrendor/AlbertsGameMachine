package Outpost;

public class ServerState
{
  String m_state;
  String m_purchaser;
  String m_bidder;

  public ServerState(String i_state,String i_purchaser,String i_bidder)
  {
    m_state = i_state;
    m_purchaser = i_purchaser;
    m_bidder = i_bidder;
  }

  private boolean NullStringEquals(String i_left,String i_right)
  {
    if (i_left == null && i_right == null) return true;
    if (i_left == null || i_right == null) return false;
    return i_left.equals(i_right);
  }

  public boolean equals(ServerState i_right)
  {
    if (i_right == null) return false;
    if (!NullStringEquals(m_state,i_right.m_state)) return false;
    if (!NullStringEquals(m_purchaser,i_right.m_purchaser)) return false;
    if (!NullStringEquals(m_bidder,i_right.m_bidder)) return false;
    return true;
  }

  public String toString()
  {
    return("[state="+m_state+
           ",purchaser="+m_purchaser+
           ",bidder="+m_bidder+"]");
  }
}

  
