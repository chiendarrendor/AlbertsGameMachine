package Outpost;
import java.util.Vector;
import javax.swing.JPanel;
import javax.swing.JLabel;
import NodeInterfacePackage.*;
import MySwing.ImageLoader;
import javax.swing.BoxLayout;

public class FactoryWidget
{
  private String m_DataLoc;
  private boolean m_IAM;
  private JPanel m_panel;
  private JLabel m_factorylabel;
  private JLabel m_manlabel;
  private char m_ManType;
  private char m_OrigManType;
  private char m_FactoryType;
  private int m_Index;
  private MyMouseListener m_fmm;
  
  public char GetManType() { return m_ManType; }
  
  public void SetManType(char i_ManType) { m_ManType = i_ManType; }
  public void ResetManning() { m_ManType = m_OrigManType; }
  public void SetManEnabled(boolean i_enabled) { m_manlabel.setEnabled(i_enabled); }
  
  public FactoryWidget(char i_FactoryType,char i_ManType,int i_Index,String i_DataLoc,boolean i_IAM,MyMouseListener i_fmm)
  {
    m_DataLoc = i_DataLoc;
    m_Index = i_Index;
    m_IAM = i_IAM;
    m_fmm = i_fmm;
    m_ManType = i_ManType;
    m_OrigManType = i_ManType;
    m_FactoryType = i_FactoryType;
  }
  
  public void Realize(JPanel i_Panel)
  {
    m_panel = new JPanel();
    m_panel.setOpaque(false);
    m_panel.setVisible(true);
    m_panel.setLayout(new BoxLayout(m_panel,BoxLayout.Y_AXIS));
    i_Panel.add(m_panel);
    
    MakeFactoryLabel();
    MakeManLabel();
  }
  
  public int GetFactoryMeanValue()
  {
    switch(m_FactoryType)
    {
    case 'O': return 3;
    case 'W': return 7;
    case 'T': return 10;
    case 'R': return 13;
    case 'N': return 20;
    case 'S': return 30;
    case 'P': return 40;
    case 'M': return 50;
    }
    return 0;
  }
  
  void MakeFactoryLabel()
  {
    switch(m_FactoryType)
    {
    case 'O': m_factorylabel = ImageLoader.MakeImageLabel(m_DataLoc + "/Ore.PNG"); break;
    case 'W': m_factorylabel = ImageLoader.MakeImageLabel(m_DataLoc + "/Water.PNG"); break;
    case 'T': m_factorylabel = ImageLoader.MakeImageLabel(m_DataLoc + "/Titanium.PNG"); break;
    case 'R': m_factorylabel = ImageLoader.MakeImageLabel(m_DataLoc + "/Research.PNG"); break;
    case 'N': m_factorylabel = ImageLoader.MakeImageLabel(m_DataLoc + "/NewChemicals.PNG"); break;
    case 'S': m_factorylabel = ImageLoader.MakeImageLabel(m_DataLoc + "/OrbitalMedicine.PNG"); break;
    case 'P': m_factorylabel = ImageLoader.MakeImageLabel(m_DataLoc + "/RingOre.PNG"); break;
    case 'M': m_factorylabel = ImageLoader.MakeImageLabel(m_DataLoc + "/MoonOre.PNG"); break;
    default: m_factorylabel = ImageLoader.MakeImageLabel(m_DataLoc + "/Empty.PNG"); break;
    }
    m_panel.add(m_factorylabel);
    
  }
  
  public void UpdateManLabel()
  { 
    switch(m_ManType)
    {
    case 'H': m_manlabel.setIcon(ImageLoader.MakeImageIcon(m_DataLoc + "/Man.PNG")); break;
    case 'B': m_manlabel.setIcon(ImageLoader.MakeImageIcon(m_DataLoc + "/Robot.PNG")); break;
    default: m_manlabel.setIcon(ImageLoader.MakeImageIcon(m_DataLoc + "/Empty.PNG")); break;
    }
  }    

  
  private void MakeManLabel()
  {
    m_manlabel = new MouseAwareJLabel(m_Index,m_fmm);
    m_panel.add(m_manlabel);
    
    UpdateManLabel();
  }

  private static FactoryWidget ParseFactory(String i_String,int i_Index,String i_DataLoc,boolean i_IAM,MyMouseListener i_fmm)
  {
    if (i_String.length() != 2) throw new NodeInterfacePackageException("Bad Factory String: " + i_String);
    char f = i_String.charAt(0);
    char m = i_String.charAt(1);

    if ((f == 'O' ||
         f == 'W' ||
         f == 'T' ||
         f == 'R' ||
         f == 'N' ||
         f == 'S' ||
         f == 'P' ||
         f == 'M') &&
        (m == 'U' ||
         m == 'H' ||
         m == 'B'))
    {
      return new FactoryWidget(f,m,i_Index,i_DataLoc,i_IAM,i_fmm);
    }
    
    throw new NodeInterfacePackageException("Unknown chars in factory string: " + i_String);
  }

  public static Vector<FactoryWidget> ParseFactoryString(String i_string,String i_DataLoc,boolean i_IAM,MyMouseListener i_fmm)
  {
    Vector<FactoryWidget> result = new Vector<FactoryWidget>();

    if (i_string.charAt(0) != '|') throw new NodeInterfacePackageException("Factory String doesn't start with |");
    if (i_string.charAt(i_string.length()-1) != '|') 
      throw new NodeInterfacePackageException("Factory String doesn't end with |");

    String sub = i_string.substring(1,i_string.length()-1);
    if (sub.length() == 0) return result;

    String[] pvec = sub.split("-");

    int i;

    for (i = 0 ; i < pvec.length ; ++i)
    {
      result.add(ParseFactory(pvec[i],i,i_DataLoc,i_IAM,i_fmm));
    }

    return result;
  }


}

