package MerchantOfVenus;

import java.awt.event.*;
import java.awt.*;
import javax.swing.*;
import java.util.*;
import NodeInterfacePackage.*;

public class TradeItem extends JPanel implements ActionListener
{
	private static final long serialVersionUID = 1L;
ActionTransferManager atm;
  String varName;
  String actName;
  Token token;

  public TradeItem(ActionTransferManager i_atm,Token i_token,int count,
                   String i_varname,String i_actname,
                   String buttonText,Vector<Note> notes,boolean hasButton,
                   TradeItemFactory.TradeType type)
  {
    setBackground(Color.BLACK);
    setLayout(new BoxLayout(this,BoxLayout.Y_AXIS));
    setAlignmentY(Component.TOP_ALIGNMENT);
    atm = i_atm;
    token = i_token;
    actName = i_actname;
    varName = i_varname;

    String cstring = " ";
    if (type == TradeItemFactory.TradeType.BASETYPE && token.IsUnlimited())
    {
      cstring = "unlimited";
    }
    else if (count > 1)
    {
      cstring = ""+count+"x";
    }

    add(ControlPanel.MakeDecoratedLabel(cstring));
    add(Box.createRigidArea(new Dimension(0,5)));

    add(new JLabel(IconManager.GetTokenIcon(token)));
    
    if (notes != null)
    {
      for (Note n : notes) 
      {
        Color fg;
        String prefix;
        add(Box.createRigidArea(new Dimension(0,5)));
        switch(n.type)
        {
        case Note.BASENOTE: fg = Color.WHITE; prefix = ""; break;
        case Note.DISCOUNTNOTE: fg = Color.ORANGE; prefix = "- "; break;
        case Note.BONUSNOTE: fg = Color.GREEN; prefix = "+ "; break;
        case Note.FINALNOTE: fg = Color.WHITE; prefix = "= "; break;
        case Note.NOTNOTE: fg = Color.RED; prefix = "X "; break;
        case Note.COMMISSIONNOTE: fg = Color.BLUE; prefix = "=> "; break;
        default: fg = Color.WHITE; prefix = "? " ; break;
        }
        JLabel theNote = new JLabel(prefix + n.note);
        theNote.setForeground(fg);
        add(theNote);
      }
    }
    
    if (hasButton)
    {
      JButton jb = new JButton(buttonText);
      add(Box.createRigidArea(new Dimension(0,5)));
      add(jb);
      jb.addActionListener(this);
    }

    for (Component c : getComponents()) 
    { 
      JComponent jc = (JComponent)c;
      jc.setAlignmentX(Component.CENTER_ALIGNMENT); 
    }

  }


  public void actionPerformed(ActionEvent e)
  {
    if (varName != null)
    {
      atm.ReportNewValueToConsumers(varName,token.GetEncoding());
    }
    atm.ButtonPressed(actName);
  }
}
    
    