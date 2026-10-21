package Outpost;
import javax.swing.JPanel;
import javax.swing.JLabel;
import java.util.HashMap;
import NodeInterfacePackage.*;

public class CommodityMegaSelector implements UserDefinedInterface
{
    JLabel m_iconlabel = new JLabel();

    public CommodityMegaSelector(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ActionTransferManager i_atm)
    {
        i_Panel.add(m_iconlabel);
    }

    private GuiPacketQuery resourceTypeQuery = new GuiPacketQuery("megaRequest.resourceType");
    public void HandleEvent(GameEvent i_event)
    {
        int resourceType = resourceTypeQuery.evaluateInt(i_event,-1);
        if (resourceType == -1) return;

        int megaSize = -1;
        switch(resourceType) {
            case 2: megaSize = 30; break;
            case 3: megaSize = 44; break;
            case 4: megaSize = 56; break;
            case 6: megaSize = 88; break;
            default: throw new NodeInterfacePackageException("Unknown Mega Resource Type " + resourceType);
        }

        m_iconlabel.setIcon(Commodity.getIcon(resourceType, Integer.toString(megaSize), true, false, false));
    }

    public void Refresh()
    {
    }

    public void Reset()
    {
        m_iconlabel.setIcon(null);
    }

}