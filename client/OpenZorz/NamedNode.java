package OpenZorz;

import javax.swing.*;
import java.awt.*;

import MySwing.LockLayout;

public class NamedNode extends GameNode
{
    public NamedNode(GameNode i_Parent,int i_x,int i_y,String i_hidden)
    {
	super(i_Parent,i_hidden);
	Point p = new Point(i_x,i_y);

	JPanel panel = new JPanel();
	LockLayout layout = new LockLayout();
	panel.setLayout(layout);
	panel.setOpaque(false);

	i_Parent.AddComponent(panel,p);

	panel.setVisible(IsVisible());

	SetComponent(panel);
    }
}
