package MerchantOfVenus;

import java.util.*;
import NodeInterfacePackage.*;

public class OverrideSpaceData
{
  public interface SpaceListener
  {
    public void SpaceChanged(Override o);
    public void Reset();
  }

  public class Override
  {
    public String secrecy;
    public Board.MapSpace mapspace;
    public IconManager.Decoding icon;
    public Override(Board.MapSpace ms) { secrecy = "" ; mapspace = ms; icon = null; }
  }

  private Map<String,Override> overrides;
  public Map<String,Override> GetOverrides() { return overrides; }
  private Vector<SpaceListener> listeners;
  private Board board;

  public void AddSpaceListener(SpaceListener sl) { listeners.add(sl); }

  public OverrideSpaceData(Board b)
  {
    board = b;
    listeners = new Vector<SpaceListener>();
    Reset();
  }

  public void Reset()
  {
    overrides = new HashMap<String,Override>();
    for (SpaceListener sl : listeners) { sl.Reset(); }
  }

  public void HandleEvent(GameEvent ev)
  {
    Override theOverride = null;

    if (ev.GetVarValue("eventname").equals("QBOXPRIVACY"))
    {
      String sname = ev.GetVarValue("QBOXPRIVACY.SpaceName");
      if (!overrides.containsKey(sname))
      {
        Override o = new Override(board.GetSpaces().get(sname));
        overrides.put(sname,o);
      }
      theOverride = overrides.get(sname);
      theOverride.secrecy = ev.GetVarValue("QBOXPRIVACY.Secrecy");
    }
    if (ev.GetVarValue("eventname").equals("QBOXCONTENTS"))
    {
      String sname = ev.GetVarValue("QBOXCONTENTS.SpaceName");
      String contents = ev.GetVarValue("QBOXCONTENTS.SpaceEncoding");
      if (!overrides.containsKey(sname))
      {
        Override o = new Override(board.GetSpaces().get(sname));
        overrides.put(sname,o);
      }
      theOverride = overrides.get(sname);
      theOverride.icon = IconManager.DecodeSpaceEncoding(contents);
    }

    if (theOverride != null) for (SpaceListener sl : listeners) sl.SpaceChanged(theOverride);
  }
}
