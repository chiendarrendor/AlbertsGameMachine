package MerchantOfVenus;
import java.util.*;

class SolarSystemHolderDomain 
{
    private Map<String,SolarSystemHolder> solarsystems = new HashMap<String,SolarSystemHolder>();
    public Map<String,SolarSystemHolder> GetSolarSystemHolders() { return solarsystems; }

    public SolarSystemHolderDomain(String dataloc)
    {
	Board theBoard = new Board(dataloc);
	solarsystems = new HashMap<String,SolarSystemHolder>();
	for (Map.Entry<String,Board.SolarSystem> ssent : theBoard.GetSolarSystems().entrySet())
	{
	    String ssname = ssent.getKey();
	    Board.SolarSystem ss = ssent.getValue();
	    if (!ss.IsHabitable()) continue;
	    SolarSystemHolder ssh = new SolarSystemHolder();
	    solarsystems.put(ssname,ssh);
	    for (Board.MapSpace ms : ss.GetNonLoopSpaces())
	    {
		ssh.AddPoint(ms.GetX(),ms.GetY());
	    }
	    for (int i = 1 ; i <= ss.GetNumLoopSpaces() ; ++i)
	    {
		Board.MapSpace ms = ss.GetLoopSpace(i);
		ssh.AddPoint(ms.GetX(),ms.GetY());
	    }
	    ssh.execute();
	}
    }
}	