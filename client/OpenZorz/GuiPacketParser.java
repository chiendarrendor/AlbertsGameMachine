package OpenZorz;



class GuiPacketParser
{
  public static final int NOTGUI = 0;
  public static final int NEWGUI = 1;
  public static final int ADDGUI = 2;
  public static final int DROPGUI = 3;
  public static final int RESETGUI = 4;
  public static final int MESSAGE = 5;
  public static final int SERVERERROR = 6;
  public static final int ERRORGUI = 1000;

  private int m_guitype;
  private String m_guiname;
  private String m_xmlloc;
  private String m_xmlname;
  private String m_text;
  
  public int GetGuiType() { return m_guitype; }
  public String GetGuiName() { return m_guiname; }
  public String GetXMLLoc() { return m_xmlloc; }
  public String GetXMLName() { return m_xmlname; }
  public String GetText() { return m_text; }

  public GuiPacketParser(String i_string)
  {
    m_guitype = NOTGUI;
    m_guiname = null;
    m_xmlloc = null;
    m_xmlname = null;

    String[] tokens = i_string.split(",");
    if (tokens.length == 0)
    {
      m_guitype = ERRORGUI;
      return;
    }
    
    if (tokens[0].equals("NEWGUI"))
    {
      if (tokens.length != 4)
      {
        m_guitype = ERRORGUI;
        return;
      }

      m_guitype = NEWGUI;
      m_guiname = tokens[1];
      m_xmlloc = tokens[2];
      m_xmlname = tokens[3];
    }
    else if (tokens[0].equals("ADDGUI"))
    {
      if (tokens.length != 4)
      {
        m_guitype = ERRORGUI;
        return;
      }
      m_guitype = ADDGUI;
      m_guiname = tokens[1];
      m_xmlloc = tokens[2];
      m_xmlname = tokens[3];
    }
    else if (tokens[0].equals("DROPGUI"))
    {
      if (tokens.length != 2)
      {
        m_guitype = ERRORGUI;
        return;
      }
      m_guitype = DROPGUI;
      m_guiname = tokens[1];
    }
    else if (tokens[0].equals("RESET"))
    {
      if (tokens.length != 2)
      {
        m_guitype = ERRORGUI;
        return;
      }
      m_guitype = RESETGUI;
      m_guiname = tokens[1];
    }
    /* disabling message so that a gui can handle player messages
    else if (tokens[0].equals("MESSAGE"))
    {
      if (tokens.length != 2)
      {
        m_guitype = ERRORGUI;
        return;
      }

      m_guitype = MESSAGE;
      m_text = tokens[1];
    }
    */
    else if (tokens[0].equals("ERROR"))
    {
      if (tokens.length != 2)
      {
        m_guitype = ERRORGUI;
        return;
      }

      m_guitype = SERVERERROR;
      m_text = tokens[1];
    }
  }
}
             
  
