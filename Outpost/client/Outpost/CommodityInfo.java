package Outpost;

import java.util.Vector;
import NodeInterfacePackage.NodeInterfacePackageException;

public class CommodityInfo
{
  private int m_commType;
  private String m_value;
  private boolean m_isMega;
  private boolean m_isAvg;
  private boolean m_discarded;

  public int GetCommType() { return m_commType; }
  public String GetValueString() { return m_value; }
  public boolean IsMega() { return m_isMega; }
  public boolean IsAvg() { return m_isAvg; }

  public boolean Discarded() { return m_discarded; }
  public void SetDiscardState(boolean i_state) { m_discarded = i_state; }



  public int GetWeight()
  {
    return m_isMega ? 4 : 1;
  }

  private int GetValueOfType()
  {
    switch (m_commType)
    {
    case 1: return 3;
    case 2: return 7;
    case 3: return 10;
    case 4: return 13;
    case 5: return 17;
    case 6: return 20;
    case 7: return 30;
    case 8: return 40;
    case 9: return 50;
    default: return 0;
    }
  }


  public int GetMeanValue()
  {
    if (IsAvg() || IsMega())
    {
      return Integer.parseInt(m_value);
    }
    
    return GetValueOfType();
  }

  public int GetValue()
  {
    if (m_value.equals("?"))
    {
      return GetValueOfType();
    }

    return Integer.parseInt(m_value);
  }

  public CommodityInfo(String i_string)
  {
    m_discarded = false;
    char lastchar;
    boolean hassuffix = false;
    String vstring;
    
    if (i_string.length() < 3) throw new NodeInterfacePackageException("Comm String isn't long enough!");

    lastchar = i_string.charAt(i_string.length()-1);
    if (lastchar == 'M' || lastchar == 'A')
    {
      hassuffix = true;
      if (i_string.length() < 4) throw new NodeInterfacePackageException("Comm string isn't long enough to have suffixes!");
      if (lastchar == 'M') m_isMega = true;
      if (lastchar == 'A') m_isAvg = true;
      vstring = i_string.substring(2,i_string.length() - 1);
    }
    else
    {
      if (i_string.length() < 3) throw new NodeInterfacePackageException("Comm String isn't long enough!");
      vstring = i_string.substring(2,i_string.length());
    }

    if (vstring.equals("*"))
    {
      m_value = "?";
    } 
    else
    {
      m_value = vstring;
    }

    String tstring = i_string.substring(0,2);

    if      (tstring.equals("OR")) m_commType = 1;
    else if (tstring.equals("WA")) m_commType = 2;
    else if (tstring.equals("TI")) m_commType = 3;
    else if (tstring.equals("RE")) m_commType = 4;
    else if (tstring.equals("MI")) m_commType = 5;
    else if (tstring.equals("NE")) m_commType = 6;
    else if (tstring.equals("OM")) m_commType = 7;
    else if (tstring.equals("RO")) m_commType = 8;
    else if (tstring.equals("MO")) m_commType = 9;
    else throw new NodeInterfacePackageException("Unknown Commodity Type: " + tstring);
  }

  static public Vector<CommodityInfo> ParseCommodityString(String i_string)
  {
    Vector<CommodityInfo> result = new Vector<CommodityInfo>();

    if (i_string.charAt(0) != '|') throw new NodeInterfacePackageException("Comm String doesn't start with |");
    if (i_string.charAt(i_string.length()-1) != '|') 
      throw new NodeInterfacePackageException("Comm String doesn't end with |");

    String sub = i_string.substring(1,i_string.length()-1);
    if (sub.length() == 0) return result;

    String[] pvec = sub.split("-");

    int i;

    for (i = 0 ; i < pvec.length ; ++i)
    {
      result.add(new CommodityInfo(pvec[i]));
    }

    return result;
  }
}
