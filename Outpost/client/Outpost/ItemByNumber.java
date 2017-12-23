package Outpost;

import java.util.HashMap;
import javax.swing.JLabel;
import NodeInterfacePackage.*;
import java.awt.Color;

public class ItemByNumber
{
  static HashMap<Integer,DrawThing> s_drawthings = new HashMap<Integer,DrawThing>();
  static boolean s_DoOnce = false;

  static public JLabel createJLabel(int i_cardnum)
  {
    JLabel jl = new JLabel();
    getDrawThing(i_cardnum).setJLabel(jl);
    return jl;
  }

  static public void alterJLabel(JLabel i_jlabel,int i_cardnum)
  {
    i_jlabel.setToolTipText(null);
    getDrawThing(i_cardnum).setJLabel(i_jlabel);
  }



  static private DrawThing getDrawThing(int i_cardnum)
  {
    if (!s_DoOnce) throw new NodeInterfacePackageException("uninitialized ItemByNumber!");
    if (!s_drawthings.containsKey(i_cardnum)) 
      throw new NodeInterfacePackageException("no such item: " + i_cardnum);

    return s_drawthings.get(i_cardnum);
  }

  static public void DoOnce(String i_DataLoc)
  {
    if (s_DoOnce) return;
    s_DoOnce = true;
    
    s_drawthings.put(-2,new HighItem(Color.GREEN));
    s_drawthings.put(-1,new HighItem(Color.RED));
    s_drawthings.put( 0,new NonItem());
    s_drawthings.put( 1,new Item(i_DataLoc,1,"DATA\nLIBRARY",15,"1","DataLibrary"));
    s_drawthings.put( 2,new Item(i_DataLoc,2,"WAREHOUSE",25,"1","Warehouse"));
    s_drawthings.put( 3,new Item(i_DataLoc,3,"HEAVY\nEQUIPMENT",30,"1","HeavyEquipment"));
    s_drawthings.put( 4,new Item(i_DataLoc,4,"NODULE",25,"2","Nodule"));
    s_drawthings.put( 5,new Item(i_DataLoc,5,"SCIENTISTS",40,"2","Scientists"));
    s_drawthings.put( 6,new Item(i_DataLoc,6,"ORBITAL\nLAB",50,"3","OrbitalLab"));
    s_drawthings.put( 7,new Item(i_DataLoc,7,"ROBOTS",50,"3","Robots"));
    s_drawthings.put( 8,new Item(i_DataLoc,8,"LABORATORY",100,"5","Laboratory"));
    s_drawthings.put( 9,new Item(i_DataLoc,9,"ECOPLANTS",50,"5","Ecoplants"));
    s_drawthings.put(10,new Item(i_DataLoc,10,"OUTPOST",100,"5","Outpost"));
    s_drawthings.put(11,new Item(i_DataLoc,11,"SPACE\nSTATION",120,"10 *","SpaceStation"));
    s_drawthings.put(12,new Item(i_DataLoc,12,"PLANETARY\nCRUISER",160,"15 *","PlanetaryCruiser"));
    s_drawthings.put(13,new Item(i_DataLoc,13,"MOON\nBASE",200,"20 *","MoonBase"));

    s_drawthings.put( 71,new Item(i_DataLoc,7,"ROBOTS",50,"3","Robots_1"));
    s_drawthings.put( 72,new Item(i_DataLoc,7,"ROBOTS",50,"3","Robots_2"));
    s_drawthings.put( 73,new Item(i_DataLoc,7,"ROBOTS",50,"3","Robots_3"));

  }
}

    


