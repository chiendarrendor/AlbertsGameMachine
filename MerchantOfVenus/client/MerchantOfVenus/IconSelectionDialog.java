package MerchantOfVenus;

import java.awt.*;

import javax.swing.*;
import java.util.*;
import java.awt.event.*;
import MySwing.*;

public class IconSelectionDialog extends JDialog
{
	private static final long serialVersionUID = 1L;

// called with the index of whatever button was pressed
  public interface ButtonPressListener
  {
    public void ButtonPressed(int buttonidx);
  }

  private class IndexActionListener implements ActionListener
  {
    private int idx;
    private JDialog parent;
    public IndexActionListener(JDialog par, int i) { parent = par; idx = i; }
    public void actionPerformed(ActionEvent e)
    {
      parent.setVisible(false);
      listener.ButtonPressed(idx);
    }
  }

  JLabel label;
  JButton[] buttons;
  ButtonPressListener listener;

  // buttoncontents must be either an ImageIcon or a String
  public IconSelectionDialog(String icondir,String prompt,String dialogHeader,Object[] buttonContents,ButtonPressListener bpl)
  {
    super((Frame)null,dialogHeader);

    //    setUndecorated(true);
    setResizable(false);
    setLocationRelativeTo(null);

    listener = bpl;

    ImageIcon invis = ImageLoader.MakeImageIcon(icondir+"/invisible.png");

    label = new JLabel(prompt);
    label.setHorizontalAlignment(JLabel.CENTER);
    label.setFont(label.getFont().deriveFont(Font.PLAIN,14.0f));

    buttons = new JButton[buttonContents.length];

    JPanel cPane = new JPanel(new BorderLayout());
    cPane.setOpaque(true);
    cPane.add(label,BorderLayout.CENTER);
    cPane.add(new JLabel(invis),BorderLayout.NORTH);
    cPane.add(new JLabel(invis),BorderLayout.WEST);
    cPane.add(new JLabel(invis),BorderLayout.EAST);

    JPanel buttonPane = new JPanel();
    cPane.add(buttonPane,BorderLayout.SOUTH);
    buttonPane.setLayout(new BoxLayout(buttonPane,BoxLayout.LINE_AXIS));
    buttonPane.setBorder(BorderFactory.createEmptyBorder(0,0,5,5));
    buttonPane.add(Box.createHorizontalGlue());

    int idx = 0;
    for (Object o : buttonContents)
    {
      if (o instanceof String)
      {
        String s = (String)o;
        JButton jb = new JButton(s);
        jb.addActionListener(new IndexActionListener(this,idx));
        buttons[idx] = jb;
        buttonPane.add(jb);
        buttonPane.add(Box.createHorizontalGlue());
        idx++;
      }
      else if (o instanceof ImageIcon)
      {
        ImageIcon ii = (ImageIcon)o;
        JButton jb = new JButton(ii);
        jb.addActionListener(new IndexActionListener(this,idx));
        buttons[idx] = jb;
        buttonPane.add(jb);
        buttonPane.add(Box.createHorizontalGlue());
        idx++;
      }
      else
      {
        throw new IllegalArgumentException("unknown object passed to IconSelectionDialog");
      }
    }
    setContentPane(cPane);
  }

  void show(Set<Integer> activeButtons)
  {
    for (int i = 0 ; i < buttons.length ; ++i)
    {
      buttons[i].setVisible(activeButtons.contains(new Integer(i)));
    }
    pack();
    setVisible(true);
  }
}
