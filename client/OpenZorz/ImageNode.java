package OpenZorz;

import javax.swing.*;
import java.awt.*;


public class ImageNode extends GameNode
{
  public ImageNode(GameNode i_Parent,int i_x,int i_y,String i_hidden,String i_image)
  {
    super(i_Parent,i_hidden);

    Point p = new Point(i_x,i_y);

    JLabel imglabel = MakeImageLabel(i_image);

    imglabel.setVisible(IsVisible());

    i_Parent.AddComponent(imglabel,p);
	
    SetComponent(imglabel);
  }
}
