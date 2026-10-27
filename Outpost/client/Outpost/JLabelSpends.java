package Outpost;

import NodeInterfacePackage.*;
import java.util.HashMap;
import java.util.List;
import java.util.ArrayList;
import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.JScrollPane;
import javax.swing.JScrollBar;
import java.awt.BorderLayout;
import java.awt.Component;
import javax.swing.BoxLayout;
import org.json.JSONObject;
import org.doghouse.jsonpath.JsonPath;

public class JLabelSpends implements UserDefinedInterface, SequenceManager.SequenceCallback
{
    JPanel m_boxpanel;
    JScrollBar m_jsb;
    List<JLabel> m_itemLabels = new ArrayList<>();
    SequenceManager m_sqm;

    private static final String dummyLabel = "<html>Dummy Spend<br>Dummy Resources</html>";
    private static final String sequenceName = "Spends";

    String[] buyItems = { "robot", "man",  "ore factory", "water factory", "titanium factory", "research factory", "new chemicals factory" };

    String parseBuyItem(int item)
    {
        if (item < 0 || item >= buyItems.length) return "???";
        return buyItems[item];
    }

    String[] bidItems = { 
        "???", "data library", "warehouse", "heavy equipment", "nodule", "scientists", "orbital lab", "robots", "laboratory",
        "ecoplants", "outpost", "space station", "planetary cruiser", "moon base"};

    String parseBidItem(int item)
    {
        if (item < 0 || item >= bidItems.length) return "???";
        return bidItems[item];
    }




    public JLabelSpends(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ExternalAssets i_assets)
    {
        m_sqm = i_assets.GetSequenceManager();
        i_Panel.setLayout(new BorderLayout());
        m_boxpanel = new JPanel();
        m_boxpanel.setLayout(new BoxLayout(m_boxpanel, BoxLayout.Y_AXIS));
        JScrollPane jsp = new JScrollPane(m_boxpanel);
        m_jsb = jsp.getVerticalScrollBar();
        i_Panel.add(jsp,BorderLayout.CENTER);

        m_sqm.register(this, sequenceName);
    }

    // set setAlignmentX(Component.LEFT_ALIGNMENT on every item that goes into container.

    

    public void sequenceCountIncrement(int newCount) {
        if (newCount <= m_itemLabels.size()) return;    // array = a b c, newCount = 3
        int newStartIndex = m_itemLabels.size();   // if current array is a b c, (012) we want to request items starting from 3 = current size;
        int upsize = newCount - m_itemLabels.size(); // if current array is a b c, and newcount = 5, we want to add 2.
        for (int i = 0 ; i < upsize ; ++i) {
            JLabel newLabel = new JLabel(dummyLabel);
            newLabel.setAlignmentX(Component.LEFT_ALIGNMENT);
            m_itemLabels.add(newLabel);
            m_boxpanel.add(newLabel);
        }
        m_jsb.setValue(m_jsb.getMaximum());
        Refresh();
        
        m_sqm.requestItems(sequenceName, newStartIndex, newCount);
    }

    private static JsonPath turnNumberPath = JsonPath.compile("turnNum");
    private static JsonPath playerNumberPath = JsonPath.compile("playerId");
    private static JsonPath spendTypePath = JsonPath.compile("spendType");
    private static JsonPath spendItemPath = JsonPath.compile("spendItem");
    private static JsonPath spendCostPath = JsonPath.compile("spendCost");
    private static JsonPath spendResourcePath = JsonPath.compile("resources");


    public void sequenceGetItem(int index, JSONObject itemobj) {
        int turn = (Integer)turnNumberPath.evaluate(itemobj);
        int player = (Integer)playerNumberPath.evaluate(itemobj);
        int type = (Integer)spendTypePath.evaluate(itemobj);
        int item = (Integer)spendItemPath.evaluate(itemobj);
        int cost = (Integer)spendCostPath.evaluate(itemobj);
        String res = (String)spendResourcePath.evaluate(itemobj);

        String line1;

        switch(type) {
        case 0: // item purchase
            line1 = "bought " + parseBuyItem(item) + " for " + cost;
            break;
        case 1: // bit purchase
            line1 = "won bid on " + parseBidItem(item) + " for a final price of " + cost;
            break;
        case 2: // discard
            line1 = "discarded at least " + cost + " cards";
            break;
        case 3: // water mulligan
            line1 = "spent all resources for a water factory";
            break;
        default:
            line1 = "???";
        }

        JLabel theLabel = m_itemLabels.get(index);
        theLabel.setText("<html>Turn " + turn + " Player " + player + " " + line1 + "<br>" + res + "</html>");
    }


    public void HandleEvent(GameEvent i_event) {
        // we're using the sequence api for contents, there's nothing interesting for us in the events.
    }

    public void Refresh() {
        m_boxpanel.revalidate();
        m_boxpanel.repaint();
    }

    public void Reset() {
        m_boxpanel.removeAll();
        m_jsb.setValue(0);
        Refresh();
        m_itemLabels.clear();
    }
}