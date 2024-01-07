package MySwing;

import java.io.IOException;
import java.net.URL;
import java.util.Enumeration;
import java.util.HashMap;
import javax.swing.JLabel;
import javax.swing.ImageIcon;
import java.io.File;
import java.awt.MediaTracker;



public class ImageLoader
{
  private static HashMap<String,ImageIcon> m_Icons = new HashMap<String,ImageIcon>();

  public static URL getImageURL(Class cl,String i_name) {
    return getImageURL(cl.getClassLoader(),i_name);
  }
  public static URL getImageURL(ClassLoader loader, String i_name) {
    try {
      Enumeration<URL> imageurls = loader.getResources(i_name);
      while (imageurls.hasMoreElements()) {
        URL url = imageurls.nextElement();
        if (url != null) return url;
      }
      return null;
    } catch (IOException e) {
      throw new RuntimeException("MakeImageIcon failed ", e);
    }
  }


  public static ImageIcon MakeImageIcon(ClassLoader loader,String i_name) {
      URL url = getImageURL(loader,i_name);
      if (url != null) {
        return new ImageIcon(url);
      }
      return null;
  }

  public static JLabel MakeImageLabel(ClassLoader loader, String i_name) {
    ImageIcon ii = MakeImageIcon(loader,i_name);

    JLabel label = new JLabel();
    label.setIcon(ii);
    label.setText("");
    return label;
  }




  public static ImageIcon MakeImageIcon(String i_Name) {
    return MakeImageIcon(ImageLoader.class,i_Name);
  }

  public static ImageIcon MakeImageIcon(Class opClass,String i_Name)
  {
    ImageIcon icon = m_Icons.get(i_Name);

    if (icon == null)
    {
      icon = MakeImageIcon(opClass.getClassLoader(),i_Name);
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

  public static JLabel MakeImageLabel(String i_name) {
    return MakeImageLabel(ImageLoader.class,i_name);
  }


  public static JLabel MakeImageLabel(Class theClass, String i_Name)
  {
    ImageIcon icon = MakeImageIcon(theClass, i_Name);

    JLabel label = new JLabel();
    label.setIcon(icon);
    label.setText("");
    return label;
  }


}
