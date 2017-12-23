package MySwing;

// this class will allow a user to easily manipulate
// an HTML table by content, without having to
// encode all the different HTML tags

import java.util.HashMap;

public class HTMLTable
{
  int m_width;
  int m_height;
  HashMap<String,CellContents> m_cellcontents;

  public class CellContents
  {
    boolean m_isheader;
    String m_backcolor;
    String m_forecolor;
    String m_halign;
    String m_text;

    public CellContents(boolean i_isheader)
    {
      m_isheader = i_isheader;
      m_backcolor = null;
      m_forecolor = null;
      m_halign = null;
      m_text = "";
    }

    public CellContents SetBackColor(String i_backcolor) { m_backcolor = i_backcolor; return this; }
    public CellContents SetForeColor(String i_forecolor) { m_forecolor = i_forecolor; return this; }
    public CellContents SetHAlign(String i_halign) { m_halign = i_halign; return this; }
    public CellContents SetText(String i_text) { m_text = i_text; return this; }

    public String GetHTML()
    {
      StringBuffer sb = new StringBuffer();
      if (m_isheader) sb.append("<th"); else sb.append("<td");
      if (m_halign != null)  sb.append(" align=\""+m_halign+"\"");
      if (m_backcolor != null) sb.append(" bgcolor=\""+m_backcolor+"\"");
      sb.append(">");

      if (m_forecolor != null) sb.append("<font color=\"" + m_forecolor + "\">");
      sb.append(m_text);
      if (m_forecolor != null) sb.append("</font>");
      
      if (m_isheader) sb.append("</th>"); else sb.append("</td>");
      return sb.toString();
    }
  }    




  private String GetCellKey(int i_x,int i_y)
  {
    return "" + i_x + "," + i_y;
  }
  
  public HTMLTable(int i_tablewidth,int i_tableheight,boolean i_columnsattop)
  {
    m_width = i_tablewidth;
    m_height = i_tableheight;
    m_cellcontents = new HashMap<String,CellContents>();
    int i,j;
    for (i = 0 ; i < m_width ; ++i)
    {
      for (j = 0 ; j < m_height ; ++j)
      {
        CellContents c = new CellContents(i_columnsattop ? (j == 0) : (i == 0));
        m_cellcontents.put(GetCellKey(i,j),c);
      }
    }
  }

  public CellContents GetCell(int i_x,int i_y)
  {
    return m_cellcontents.get(GetCellKey(i_x,i_y));
  }

  public String GetHTML()
  {
    StringBuffer sb = new StringBuffer();
    sb.append("<html>");
    sb.append("<table border=1>");

    int i,j;
    for (j = 0 ; j < m_height ; ++j)
    {
      sb.append("<tr>");
      for (i = 0 ; i < m_width ; ++i)
      {
        sb.append(m_cellcontents.get(GetCellKey(i,j)).GetHTML());
      }
      sb.append("</tr>");
    }

    sb.append("</table>");
    sb.append("</html>");
    return sb.toString();
  }
}

    
        
  
