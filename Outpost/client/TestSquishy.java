
import MySwing.SquishyPane;
import javax.swing.JFrame;
import javax.swing.border.Border;
import javax.swing.BorderFactory;
import javax.swing.JLabel;
import java.awt.Color;
import javax.swing.BoxLayout;
import javax.swing.JPanel;
import java.awt.LayoutManager;

public class TestSquishy
{
  public static void main(String argv[])
  {
    JFrame jf = new JFrame("SquishyPane Test");

    JPanel jp = new JPanel();
    LayoutManager bl = new BoxLayout(jp,BoxLayout.X_AXIS);
    jp.setLayout(bl);
      
    jf.getContentPane().add(jp);
    jp.add(new JLabel("Screen Widening String:"));
    
    
    SquishyPane sqp = new SquishyPane(2,10);
    jp.add(sqp);

    JLabel jl = new JLabel("Text1");
    jl.setOpaque(true);
    Border b = BorderFactory.createLineBorder(Color.green);
    jl.setBorder(b);
    sqp.add(jl);

    jl = new JLabel("Text2");
    jl.setOpaque(true);
    b = BorderFactory.createLineBorder(Color.red);
    jl.setBorder(b);
    sqp.add(jl);

    jl = new JLabel("Text3");
    jl.setOpaque(true);
    b = BorderFactory.createLineBorder(Color.blue);
    jl.setBorder(b);
    sqp.add(jl);

    jl = new JLabel("Text4");
    jl.setOpaque(true);
    b = BorderFactory.createLineBorder(Color.yellow);
    jl.setBorder(b);
    sqp.add(jl);

    jl = new JLabel("Text5");
    jl.setOpaque(true);
    b = BorderFactory.createLineBorder(Color.black);
    jl.setBorder(b);
    sqp.add(jl);

    jf.setVisible(true);
  }
}
