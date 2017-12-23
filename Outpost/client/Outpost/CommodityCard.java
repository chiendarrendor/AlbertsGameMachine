package Outpost;

import java.util.HashMap;
import NodeInterfacePackage.*;
import javax.swing.JPanel;

public class CommodityCard implements UserDefinedInterface
{
  public CommodityCard(HashMap<String,String> i_Options,String i_DataLoc,JPanel i_Panel,ActionTransferManager i_atm)
  {
    if (!i_Options.containsKey("commtype")) throw new NodeInterfacePackageException("missing option commtype");
    if (!i_Options.containsKey("value"))    throw new NodeInterfacePackageException("missing option value");
    if (!i_Options.containsKey("ismega"))   throw new NodeInterfacePackageException("missing option ismega");
    if (!i_Options.containsKey("isavg"))    throw new NodeInterfacePackageException("missing option isavg");
    if (!i_Options.containsKey("issel"))    throw new NodeInterfacePackageException("missing option issel");
    int commtype = Integer.parseInt(i_Options.get("commtype"));
    if (commtype < 1 || commtype > 9) throw new NodeInterfacePackageException("bad value for commtype");
    String value = i_Options.get("value");
    boolean ismega = Boolean.valueOf(i_Options.get("ismega"));
    boolean isavg = Boolean.valueOf(i_Options.get("isavg"));
    boolean issel = Boolean.valueOf(i_Options.get("issel"));

    i_Panel.add(Commodity.getJLabel(commtype,value,ismega,isavg,issel));
  }

  public void HandleEvent(GameEvent i_event)
  {
  }

  public void Refresh()
  {
  }

  public void Reset()
  {
  }
}
