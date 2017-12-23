package MySwing;

import java.util.HashMap;
import javax.swing.JLabel;
import javax.swing.ImageIcon;
import java.io.File;
import java.awt.MediaTracker;



public class ImageLoader
{
  private static HashMap<String,ImageIcon> m_Icons = new HashMap<String,ImageIcon>();

  public static ImageIcon MakeImageIcon(String i_Name)
  {
    ImageIcon icon = m_Icons.get(i_Name);

    if (icon == null)
    {
	    java.net.URL imageURL = ImageLoader.class.getResource(i_Name);

      if (imageURL != null)
      {
        icon = new ImageIcon(imageURL);
      }
    }

    if (icon == null)
    {
      java.net.URL realURL = null;
      try
      {
         realURL = new java.net.URL(i_Name);
         icon = new ImageIcon(realURL);
      }
      catch(java.net.MalformedURLException mue)
      {
        System.err.println("malformed URL " + i_Name + " ... trying as file");
      }

      if (icon != null)
      {
        if (icon.getImageLoadStatus() != MediaTracker.COMPLETE)
        {
          System.out.println("Media loading status for " + i_Name + " = " + icon.getImageLoadStatus());
          icon = null;
        }
      }
    }

    if (icon == null)
    {
      File tfile = new File(i_Name);
      if (tfile.canRead())
      {
        icon = new ImageIcon(i_Name);
      }
    }

    if (icon  == null)
    {
      System.err.println("Image " + i_Name + " not found.");
      icon = new ImageIcon();
    }

    m_Icons.put(i_Name,icon);

    return icon;
  }

  public static JLabel MakeImageLabel(String i_Name)
  {
    ImageIcon icon = MakeImageIcon(i_Name);

    JLabel label = new JLabel();
    label.setIcon(icon);
    label.setText("");
    return label;
  }


}
