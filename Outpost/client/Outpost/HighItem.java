package Outpost;

import java.awt.*;
import javax.swing.JLabel;

public class HighItem extends DrawThing
{
  public HighItem(Color i_color)
  {
    super();
    m_g2d.setColor(i_color);
    m_g2d.fillRect(0,0,width,height);
  }
}

      
