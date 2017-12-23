package OpenZorz;


import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class TextBoxNode extends ActionGeneratorNode implements ActionListener
{
  JTextField m_tfield;

  public TextBoxNode(String i_Name,int i_size,int i_x,int i_y,String i_hidden,
                     GameNode i_ParentNode,GameGui i_gamegui)
  {
    super(i_ParentNode,i_hidden,i_gamegui,i_Name);

    InitializeNode(i_ParentNode,i_size,i_x,i_y);
  }


  public void InitializeNode(GameNode i_ParentNode,int i_size,int i_x,int i_y)
  {
    if (i_size == 0) i_size = 15;

    m_tfield = new JTextField(i_size);
    m_tfield.addActionListener(this);

    Point p = new Point(i_x,i_y);
    m_tfield.setVisible(IsVisible());
    i_ParentNode.AddComponent(m_tfield,p);
    SetComponent(m_tfield);
  }
    
  public void actionPerformed(ActionEvent i_event)
  {
    VarSet(m_tfield.getText());
  }

  void DoActionReset()
  {
    DoReset();
  }

  void DoReset()
  {
    m_tfield.setText("");
  }
  
  public String ActionVariableGetValue()
  {
    return m_tfield.getText();
  }

}
