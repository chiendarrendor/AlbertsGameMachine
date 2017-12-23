import java.lang.*;
import javax.swing.*;
import java.awt.*;

class ItemGenerator
{
  private static final int height = 100;
  private static final int width = 75;
  public static void main(String argv[])
  {
    JFrame frame = new JFrame("Drawn Item");

    frame.getContentPane().setLayout(new GridLayout(1,13));
    frame.getContentPane().setBackground(Color.GREEN);

    Item item;
    JLabel jl;

    item=new Item(1,"DATA\nLIBRARY",15,"1","DataLibrary");
    frame.getContentPane().add(item.createJLabel());

    item=new Item(2,"WAREHOUSE",25,"1","Warehouse");
    frame.getContentPane().add(item.createJLabel());

    item=new Item(3,"HEAVY\nEQUIPMENT",30,"1","HeavyEquipment");
    frame.getContentPane().add(item.createJLabel());

    item=new Item(4,"NODULE",25,"2","Nodule");
    frame.getContentPane().add(item.createJLabel());

    item=new Item(5,"SCIENTISTS",40,"2","Scientists");
    frame.getContentPane().add(item.createJLabel());

    item=new Item(6,"ORBITAL\nLAB",50,"3","OrbitalLab");
    frame.getContentPane().add(item.createJLabel());

    item=new Item(7,"ROBOTS",50,"3","Robots");
    frame.getContentPane().add(item.createJLabel());

    item=new Item(8,"LABORATORY",100,"5","Laboratory");
    frame.getContentPane().add(item.createJLabel());
    
    item=new Item(9,"ECOPLANTS",50,"5","Ecoplants");
    frame.getContentPane().add(item.createJLabel());

    item=new Item(10,"OUTPOST",100,"5","Outpost");
    frame.getContentPane().add(item.createJLabel());

    item=new Item(11,"SPACE\nSTATION",120,"10 *","SpaceStation");
    frame.getContentPane().add(item.createJLabel());

    item=new Item(12,"PLANETARY\nCRUISER",160,"15 *","PlanetaryCruiser");
    frame.getContentPane().add(item.createJLabel());

    item=new Item(13,"MOON\nBASE",200,"20 *","MoonBase");
    frame.getContentPane().add(item.createJLabel());


    frame.setVisible(true);
    Insets pins = frame.getInsets();
    int w = 14*width + pins.left + pins.right + 10;
    int h = height + pins.top + pins.bottom + 10;
    frame.setSize(w,h);
    frame.setVisible(true);
    
  }
}
