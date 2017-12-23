package MerchantOfVenus;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

public class ScrollWithGlass extends JLayeredPane implements MouseMotionListener
{
	private static final long serialVersionUID = 1L;
	private JComponent child;

public ScrollWithGlass(JComponent i_child,JComponent glass)
  {
    SpringLayout springs = new SpringLayout();
    setLayout(springs);

    this.child = i_child;
    JScrollPane jsp = new JScrollPane(child);
    child.setAutoscrolls(true);
    child.addMouseMotionListener(this);
    
    JScrollBar vertical = jsp.getVerticalScrollBar();
    InputMap im = vertical.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW);
    im.put(KeyStroke.getKeyStroke("DOWN"), "positiveUnitIncrement");
    im.put(KeyStroke.getKeyStroke("UP"), "negativeUnitIncrement");

    

    jsp.getViewport().setScrollMode(JViewport.SIMPLE_SCROLL_MODE);
    jsp.getVerticalScrollBar().setUnitIncrement(15);
    jsp.getHorizontalScrollBar().setUnitIncrement(15);
    add(jsp,new Integer(0));
    add(glass,new Integer(1));

    springs.putConstraint(SpringLayout.NORTH,jsp,0,SpringLayout.NORTH,this);
    springs.putConstraint(SpringLayout.SOUTH,jsp,0,SpringLayout.SOUTH,this);
    springs.putConstraint(SpringLayout.EAST,jsp,0,SpringLayout.EAST,this);
    springs.putConstraint(SpringLayout.WEST,jsp,0,SpringLayout.WEST,this);

    int scrollw = jsp.getVerticalScrollBar().getPreferredSize().width + 1;
    int scrollh = jsp.getHorizontalScrollBar().getPreferredSize().height + 1;
    
    springs.putConstraint(SpringLayout.NORTH,glass,1,SpringLayout.NORTH,this);
    springs.putConstraint(SpringLayout.SOUTH,glass,-scrollh,SpringLayout.SOUTH,this);
    springs.putConstraint(SpringLayout.EAST,glass,-scrollw,SpringLayout.EAST,this);
    springs.putConstraint(SpringLayout.WEST,glass,1,SpringLayout.WEST,this);
  }

@Override
public void mouseDragged(MouseEvent e)
{
	Rectangle r = new Rectangle(e.getX(), e.getY(), 1, 1);
    child.scrollRectToVisible(r);
	// TODO Auto-generated method stub
	
}

@Override
public void mouseMoved(MouseEvent e)
{
	// TODO Auto-generated method stub
	
}


}

    

    
    