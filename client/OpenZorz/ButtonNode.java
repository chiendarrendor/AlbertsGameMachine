package OpenZorz;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

import MySwing.ImageLoader;
import NodeInterfacePackage.GameEvent;


public class ButtonNode 
  extends ActionGeneratorNode 
  implements ActionListener,ReplaceableTextNode
{
  JButton m_button;
  EventStringReplacer m_esp;
  String m_varval;

  public ButtonNode(String i_Name,String i_VarValue,String i_hidden,
                    int i_x,int i_y,
                    String i_image,
                    GameNode i_ParentNode,
                    GameGui i_gamegui)
  {
    super(i_ParentNode,i_hidden,i_gamegui,i_Name);

    m_esp = new EventStringReplacer();

    m_varval = i_VarValue;
    m_button = new JButton();
    m_button.addActionListener(this);
    Point p = new Point(i_x,i_y);
    m_button.setVisible(IsVisible());
    i_ParentNode.AddComponent(m_button,p);
    SetComponent(m_button);

    if (i_image != null)
    {
	    m_button.setIcon(ImageLoader.MakeImageIcon(i_gamegui.getRemoteClassLoader(),i_image));
    }
    if (m_esp.GetText() != null)
    {
	    m_button.setText(m_esp.GetText());
    }
  }

  public void AddText(String i_text)
  {
    if (i_text != null)
    {
      m_esp.AddText(i_text.trim());
      m_button.setText(m_esp.GetText());
    }
  }

  public EventStringReplacer.Replacement AddReplacer(String i_replacestring,
                                                     String i_eventvarname,
                                                     String i_initialvarvalue)
  {
    EventStringReplacer.Replacement rep = m_esp.AddReplacer(i_replacestring,i_eventvarname,i_initialvarvalue);
    if (m_esp.GetText() != null)
    {
      m_button.setText(m_esp.GetText());
    }
    return rep;
  }
    
  public void actionPerformed(ActionEvent i_event)
  {
    VarSet(m_varval);
  }

  public String ActionVariableGetValue()
  {
    return null;
  }

  public void MyHandleEvent(GameEvent i_event)
  {
    m_esp.HandleEvent(i_event);

    if (m_esp.GetText() != null)
    {
      m_button.setText(m_esp.GetText());
    }
  }

  void DoActionReset()
  {
  }

  void DoReset()
  {
    m_esp.ResetText();
    if (m_esp.GetText() != null)
    {
      m_button.setText(m_esp.GetText());
    }
  }
}
