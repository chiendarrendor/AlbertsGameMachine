package MerchantOfVenus;

import MySwing.*;
import java.awt.*;
import java.awt.image.*;
import javax.swing.*;
import java.util.*;

class IconManager
{
  private static Map<String,ImageIcon> icons = new HashMap<String,ImageIcon>();
  private static int penaltyWidth = 64;
  private static int penaltyHeight = 30;
  private static String theDir;
  private static int aindex = 0;
  private static int acount = 3;
  
  public static void Initialize(String dir) { theDir = dir; }

  // possible values of contents:
  // Telegate <id>
  // Station
  // Station <playerid>
  // Penalty [BYR] <cost>
  // Asteroid
  // Asteroid <Token Encoding>
  public static class Decoding
  {
    public ImageIcon icon;
    public int playerPortId;
  }

  public static Decoding DecodeSpaceEncoding(String contents)
  {
    Decoding result = new Decoding();
    result.icon = null;
    result.playerPortId = -1;

    // parsed[0] always contains the 'type' part of the encoded data.
    // what parsed[1] contains depends...
    String[] parsed = contents.split(" ",2);

    if (parsed[0].equals("Telegate")) 
      result.icon = GetTelegate(Integer.parseInt(parsed[1]));
    else if (parsed[0].equals("Station") && parsed.length == 1)
      result.icon = GetOpenPort();
    else if (parsed[0].equals("Station"))
      result.playerPortId = Integer.parseInt(parsed[1]);
    else if (parsed[0].equals("Penalty"))
    {
      String[] body = parsed[1].split(" ");
      Board.SpaceColor c = Board.SpaceColor.BLUE;
      if (body[0].equals("R")) c = Board.SpaceColor.RED;
      if (body[0].equals("Y")) c = Board.SpaceColor.YELLOW;
      result.icon =  GetPenaltyOval(c,Integer.parseInt(body[1]));
    }
    else if (parsed[0].equals("Asteroid") && parsed.length == 1)
      result.icon =  GetAsteroid();
    else if (parsed[0].equals("Asteroid"))
    {
      Token t = new Token(parsed[1]);
      result.icon =  GetTokenIcon(t);
    }
    return result;
  }

  public static ImageIcon GetPenaltyOval(Board.SpaceColor c,int penalty)
  {
    String pname = "";
    Color realc = Color.BLUE;

    switch(c)
    {
    case RED: 
      pname = "PenaltyRed"+penalty; 
      realc = Color.RED;
      break;
    case YELLOW: 
      pname = "PenaltyYellow"+penalty; 
      realc = Color.YELLOW;
      break;
    case BLUE: 
      pname = "PenaltyBlue"+penalty; 
      realc = Color.BLUE;
      break;
    }
    if (!icons.containsKey(pname))
    {
      Image temp = new BufferedImage(penaltyWidth+1,penaltyHeight+1,
                                     BufferedImage.TYPE_4BYTE_ABGR);
      Graphics2D g2d = (Graphics2D)temp.getGraphics();
      g2d.setColor(realc);
      g2d.setStroke(new BasicStroke(1));
      g2d.fillOval(0,0,penaltyWidth,penaltyHeight);
      g2d.setColor(Color.WHITE);
      g2d.drawOval(0,0,penaltyWidth,penaltyHeight);
      g2d.setColor(Color.BLACK);
      g2d.setFont(new Font("Monospaced",Font.BOLD,30));
      StringDrawer.DrawString(g2d,""+penalty,
                              StringDrawer.MIDDLE,StringDrawer.CENTER,
                              penaltyWidth/2,penaltyHeight/2);

      icons.put(pname,new ImageIcon(temp));
    }
    return icons.get(pname);
  }

  public static ImageIcon GetTelegate(int id)
  {
    String pname = "telegate"+id;
    if (!icons.containsKey(pname))
    {
      ImageIcon back = ImageLoader.MakeImageIcon(theDir+"/telegate.png");
      Image temp = new BufferedImage(back.getIconWidth(),
                                     back.getIconHeight(),
                                     BufferedImage.TYPE_4BYTE_ABGR);
      Graphics2D g2d = (Graphics2D)temp.getGraphics();
      back.paintIcon(null,g2d,0,0);
      g2d.setColor(Color.WHITE);
      g2d.setFont(new Font("Monospaced",Font.BOLD,30));

      StringDrawer.DrawString(g2d,""+id,
                              StringDrawer.MIDDLE,StringDrawer.CENTER,
                              back.getIconWidth()/2,back.getIconHeight()/2);
      
      icons.put(pname,new ImageIcon(temp));
    }
    return icons.get(pname);
  }

  public static ImageIcon GetAsteroid()
  {
    if (!icons.containsKey("Asteroid0"))
    {
      icons.put("Asteroid0",ImageLoader.MakeImageIcon(theDir+"/aster1.png"));
      icons.put("Asteroid1",ImageLoader.MakeImageIcon(theDir+"/aster2.png"));
      icons.put("Asteroid2",ImageLoader.MakeImageIcon(theDir+"/aster3.png"));
    }

    ImageIcon result = icons.get("Asteroid"+aindex);
    aindex = (aindex+1)%acount;
    return result;
  }

  public static ImageIcon RecolorIcon(ImageIcon orig,Color fromColor,Color toColor)
  {
    BufferedImage temp = new BufferedImage(orig.getIconWidth(),
                                           orig.getIconHeight(),
                                           BufferedImage.TYPE_4BYTE_ABGR);
    orig.paintIcon(null,temp.getGraphics(),0,0);
    for (int i = 0 ; i < temp.getWidth() ; ++i)
    {
      for (int j = 0 ; j < temp.getHeight() ; ++j)
      {
        if (temp.getRGB(i,j) == fromColor.getRGB())
          temp.setRGB(i,j,toColor.getRGB());
      }
    }
    return new ImageIcon(temp);
  }

  public static ImageIcon GetOpenPort()
  {
    String pname = "openport";
    if (!icons.containsKey(pname))
    {
      icons.put(pname,ImageLoader.MakeImageIcon(theDir+"/openport.png"));
    }
    return icons.get(pname);
  }

  public static ImageIcon GetPort(Color c)
  {
    String pname = "port" + c.getRGB();
    if (!icons.containsKey(pname))
    {
      icons.put(pname,
                RecolorIcon(ImageLoader.MakeImageIcon(theDir+"/port.png"),
                            Color.WHITE,
                            c)
                );
    }
    return icons.get(pname);
    
  }

  public static ImageIcon GetTokenIcon(Token t)
  {
    
    if (!icons.containsKey(t.GetEncoding()))
    {
      ImageIcon newI = ImageLoader.MakeImageIcon(theDir+"/testPlanet.png");
      if (t.GetSizeClass().equals("relic")) newI = MakeRelicToken(t);
      else if (t.GetSizeClass().equals("iou")) newI = MakeIOUToken(t);
      // this one is doublewide
      else if (t.GetSizeClass().equals("good")) newI = MakeGoodToken(t);
      else if (t.GetSizeClass().equals("drive")) newI = MakeDriveToken(t);
      else if (t.GetSizeClass().equals("equip")) newI = MakeEquipToken(t);
      else if (t.GetSizeClass().equals("ship")) newI = MakeShipToken(t);
      else if (t.GetSizeClass().equals("fare")) newI = MakeFareToken(t);
      else if (t.GetSizeClass().equals("demand")) newI = MakeDemandToken(t);
      else if (t.GetSizeClass().equals("deed")) newI = MakeDeedToken(t);

      icons.put(t.GetEncoding(),newI);
    }
    return icons.get(t.GetEncoding());
  }

  public static ImageIcon MakeGenericToken(Token t,Color bgcolor,Color textcolor,
                                           int width,int height,String[] lines)
  {
    Image temp = new BufferedImage(width,height,BufferedImage.TYPE_4BYTE_ABGR);
    Graphics2D g2d = (Graphics2D)temp.getGraphics();
    g2d.setColor(bgcolor);
    g2d.fillRect(0,0,width,height);
    AddDriveDot(t,g2d);
    if (t.IsNew())
    {
      g2d.setColor(new Color(0,255,0));
      g2d.drawRect(0,0,width,height);
      g2d.drawRect(1,1,width-2,height-2);
    }
    g2d.setColor(textcolor);
    int cx = width/2;
    int dy = height / lines.length;
    int toff = 13;
    for (int i = 0 ; i < lines.length ; ++i)
    {
      StringDrawer.DrawString(g2d,lines[i],StringDrawer.BOTTOM,StringDrawer.CENTER,cx,toff+dy*i);
    }
    return new ImageIcon(temp);
  }

  private static void AddDriveDot(Token t,Graphics2D g2d)
  {
    int r = 15;
    int xc = 32;
    int yc = 32;
    if (t.GetSkipColors().equals("Y"))
    {
      g2d.setColor(Color.YELLOW);
      g2d.fillOval(xc-r,yc-r,r*2,r*2);
    }
    else if (t.GetSkipColors().equals("R"))
    {
      g2d.setColor(Color.RED);
      g2d.fillOval(xc-r,yc-r,r*2,r*2);
    }
    else if (t.GetSkipColors().equals("YR"))
    {
      g2d.setColor(Color.YELLOW);
      g2d.fillOval(xc-r,yc-r,r*2,r*2);
      g2d.setColor(Color.RED);
      g2d.fillArc(xc-r,yc-r,r*2,r*2,90,-180);
    }
  }

  public static ImageIcon MakeDeedToken(Token t)
  {
    String[] ar;
    if (t.GetDeedType().equals("factory"))
    {
      ar = new String[4];
      ar[0] = "FACTORY";
      String[] s = t.GetName().split(" \\(");
      ar[1] = s[0];
      ar[2] = "(" + s[1];
      ar[3] = "$" + t.GetBuyPrice();
    }
    else
    {
      ar = new String[3];
      ar[0] = "SPACEPORT";
      ar[1] = t.GetName();
      ar[2] = "$" + t.GetBuyPrice();
    }
    return MakeGenericToken(t,new Color(0xFF,0xF8,0xC6),Color.BLACK,64+4+64,64,ar);
  }

  public static ImageIcon MakeDemandToken(Token t)
  {
    String[] ar = new String[3];
    ar[0] = "DEMAND FOR";
    ar[1] = t.GetName();
    ar[2] = "+ $" + t.GetSellPrice();
    return MakeGenericToken(t,new Color(0xFF,0xFF,0x00),Color.BLACK,64+4+64,64,ar);
  }
    


  public static ImageIcon MakeFareToken(Token t)
  {
    String[] ar = new String[3];
    ar[0] = "FARE TO";
    ar[1] = t.GetBuyerRaceIds().elementAt(0);
    ar[2] = "$" + t.GetSellPrice();
    return MakeGenericToken(t,new Color(0xFF,0xFF,0x00),Color.BLACK,64,64,ar);
  }


  public static ImageIcon MakeShipToken(Token t)
  {
    String[] ar = new String[4];
    ar[0] = t.GetName();
    ar[1] = "Speed " + t.GetSpeed();
    ar[2] = "Holds: " + t.GetHoldCount();
    ar[3] = "$" + t.GetBuyPrice() + "/" + "$" + t.GetSellPrice();
    return MakeGenericToken(t,new Color(0x00,0xFF,0xFF),Color.BLACK,64,64,ar);
  }

  public static ImageIcon MakeEquipToken(Token t)
  {
    String[] ar = (t.GetName() + " $" + t.GetBuyPrice()).split(" ");
    return MakeGenericToken(t,new Color(0x00,0x80,0x00),Color.WHITE,64,64,ar);
  }

  public static ImageIcon MakeDriveToken(Token t)
  {
    String[] ar = (t.GetName() + " $" + t.GetBuyPrice()).split(" ");
    return MakeGenericToken(t,new Color(0xA5,0x2A,0x2A),Color.BLACK,64,64,ar);
  }

  public static ImageIcon MakeRelicToken(Token t)
  {
    String[] ar = ("RELIC " + t.GetName()+" $"+t.GetBuyPrice()).split(" ");
    return MakeGenericToken(t,new Color(0x80,0x00,0x80),Color.BLACK,64,64,ar);
  }

  public static ImageIcon MakeGoodToken(Token t)
  {
    String[] ar = new String[4];
    ar[0] = (t.IsFactoryGood() ? "FACTORY " : "") + "GOOD";
    ar[1] = t.GetName();
    ar[2] = Utilities.join(",",t.GetBuyerRaceIds());
    ar[3] = "$" + t.GetBuyPrice() + "/" + "$" + t.GetSellPrice();
    return MakeGenericToken(t,new Color(0xC0,0xC0,0xC0),Color.BLACK,64+4+64,64,ar);
  }

  public static ImageIcon MakeIOUToken(Token t)
  {
    String[] ar = ("IOU " + Utilities.GetRaceOfRaceId(t.GetBuyerRaceIds().elementAt(0))+" $"+t.GetSellPrice()).split(" ");
    return MakeGenericToken(t,new Color(0x34,0x72,0x35),Color.WHITE,64,64,ar);
  }


}

    



