package TheGameMachine;
import java.util.*;

import NodeInterfacePackage.GameEvent;
import NodeInterfacePackage.GuiPacketQuery;
import org.doghouse.jsonpath.JsonQueryException;
import Utilities.StringUtility;

class EventStringReplacer
{
  private String m_BaseText;
  private String m_LastText;

  public class Replacement
  {
    public String m_TextToReplace;
    public String m_EventVarName;
    public String m_InitialValue;
    public String m_CurValue;
    private GuiPacketQuery m_Query;

    public Replacement (String i_TextToReplace,
                        String i_EventVarName,
                        String i_InitialValue)
    {
      m_TextToReplace = i_TextToReplace;
      m_EventVarName = i_EventVarName;
      m_InitialValue = i_InitialValue;
      m_Query = (i_EventVarName == null) ? null : new GuiPacketQuery(i_EventVarName);
      Reset();
    }

    void Reset()
    {
      if (m_InitialValue == null)
      {
        m_CurValue = "";
      }
      else
      {
        m_CurValue = m_InitialValue;
      }
    }
  };

  private ArrayList<Replacement> m_Replacements;

  public EventStringReplacer()
  {
    m_BaseText = null;
    m_LastText = null;
    m_Replacements = new ArrayList<Replacement>();
  }

  public void AddText(String i_Text)
  {
    if (i_Text == null)
    {
      return;
    }

    if (m_BaseText == null)
    {
      m_BaseText = new String();
    }

    m_BaseText += i_Text;
    CalculateText();
  }

  public Replacement AddReplacer(String i_TextToReplace,
                                 String i_EventVarName,
                                 String i_InitialValue)
  {

    if (i_TextToReplace == null || (i_EventVarName == null && i_InitialValue == null))
    {
      return null;
    }

    Replacement rep = new Replacement(i_TextToReplace,i_EventVarName,i_InitialValue);
    m_Replacements.add(rep);
    CalculateText();
    return rep;
  }

  public String GetText()
  {
    return m_LastText;
  }

  private void CalculateText()
  {
    if (m_BaseText == null)
    {
      m_LastText = null;
    }

    m_LastText = new String(m_BaseText);

    int i;
    for (i = 0 ; i < m_Replacements.size() ; ++i)
    {
      Replacement rep = m_Replacements.get(i);

      m_LastText = StringUtility.GlobalReplaceString(m_LastText,
                                                     rep.m_TextToReplace,
                                                     rep.m_CurValue);
    }
  }

  public void ResetText()
  {
    int i;
    for (i = 0 ; i < m_Replacements.size() ; ++i)
    {
      Replacement rep = m_Replacements.get(i);
      rep.Reset();
    }
    CalculateText();
  }

  public void HandleEvent(GameEvent i_event)
  {

    int i;
    for (i = 0 ; i < m_Replacements.size() ; ++i)
    {
      Replacement rep = m_Replacements.get(i);

      if (rep.m_Query == null) continue;

      try
      {
        Object val = rep.m_Query.evaluate(i_event);
        if (val != null)
        {
          rep.m_CurValue = String.valueOf(val);
        }
      }
      catch (JsonQueryException jqe)
      {
      }
    }
    CalculateText();
  }
}

