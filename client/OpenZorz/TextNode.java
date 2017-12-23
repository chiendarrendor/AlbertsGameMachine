package OpenZorz;

import javax.swing.*;
import java.awt.*;

import NodeInterfacePackage.GameEvent;


public class TextNode extends GameNode implements ReplaceableTextNode
{
  EventStringReplacer m_esp;
  JLabel m_txtlabel;

  public TextNode(GameNode i_Parent,int i_x,int i_y,String i_hidden)
  {
    super(i_Parent,i_hidden);
    Point p = new Point(i_x,i_y);
    m_esp = new EventStringReplacer();

    m_txtlabel = new JLabel();

    if (m_esp.GetText() != null)
    {
      m_txtlabel.setText(m_esp.GetText());
    }

    m_txtlabel.setVisible(IsVisible());

    i_Parent.AddComponent(m_txtlabel,p);
	
    SetComponent(m_txtlabel);
  }

  public void AddText(String i_text)
  {
    if (i_text != null)
    {
      m_esp.AddText(i_text.trim());
      m_txtlabel.setText(m_esp.GetText());
    }
  }

  public EventStringReplacer.Replacement AddReplacer(String i_replacestring,
                                                     String i_eventvarname,
                                                     String i_initialvarvalue)
  {
    EventStringReplacer.Replacement rep = m_esp.AddReplacer(i_replacestring,i_eventvarname,i_initialvarvalue);
    if (m_esp.GetText() != null)
    {
      m_txtlabel.setText(m_esp.GetText());
    }
    return rep;
  }



  public void MyHandleEvent(GameEvent i_event)
  {
    m_esp.HandleEvent(i_event);

    if (m_esp.GetText() != null)
    {
      m_txtlabel.setText(m_esp.GetText());
    }
  }

  void ResetMe()
  {
    super.ResetMe();
    m_esp.ResetText();
    
    if (m_esp.GetText() != null)
    {
      m_txtlabel.setText(m_esp.GetText());
    }
  }
}
