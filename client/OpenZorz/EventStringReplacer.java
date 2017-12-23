package OpenZorz;
import java.util.*;

import NodeInterfacePackage.GameEvent;
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

    // only use showhide and hideshow events in this handler.
    private GameEventHandler m_Handler;

    public Replacement (String i_TextToReplace,
                        String i_EventVarName,
                        String i_InitialValue)
    {
      m_TextToReplace = i_TextToReplace;
      m_EventVarName = i_EventVarName;
      m_InitialValue = i_InitialValue;
      m_Handler = null;
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

    public GameEventHandler GetEventHandler()
    {
      if (m_Handler == null)
      {
        m_Handler = new GameEventHandler();
      }
      return m_Handler;
    }

    public boolean IsValid(GameEvent i_event)
    {
      if (m_Handler == null) return true;

      m_Handler.ParseEvent(i_event);

      return m_Handler.IsVisible();
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
    GameEvent dummyevent = new GameEvent("");
    CalculateText(dummyevent);
  }


  private void CalculateText(GameEvent i_event)
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
      if (!rep.IsValid(i_event)) continue;

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

      if (rep.m_EventVarName == null) continue;
        
      String val = i_event.GetVarValue(rep.m_EventVarName);

      if (val != null)
      {
        rep.m_CurValue = val;
      }
    }
    CalculateText(i_event);
  }
}

