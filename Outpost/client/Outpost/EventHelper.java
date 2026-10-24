package Outpost;
// This class contains helper functions for some of the more common items we pull out of the Outpost GameEvent

import NodeInterfacePackage.GuiPacketQuery;
import NodeInterfacePackage.GameEvent;
import org.doghouse.jsonpath.JsonPath;
import org.json.JSONObject;
import java.util.List;

public class EventHelper {
    // utility class should not be instantiable.
    private EventHelper() {}

    // looks the given player name up in the player array, and returns the id associated with that player
    // or -1 if no player names match.
    private static GuiPacketQuery playersgpq = new GuiPacketQuery("players[]");
    private static JsonPath playerNamePath = JsonPath.compile("name");
    private static JsonPath playerIdPath = JsonPath.compile("id");
    static public int getIdOfPlayer(String playerName, GameEvent i_event) {
        JSONObject player = getJSONOfPlayer(playerName,i_event);
        if (player == null) return -1;
        return (Integer)playerIdPath.evaluate(player);
    }

    @SuppressWarnings("unchecked")
    static public JSONObject getJSONOfPlayer(String playerName, GameEvent i_event) {
        List<JSONObject> players = (List<JSONObject>)playersgpq.evaluate(i_event);

        for (JSONObject player : players) {
            String name = (String)playerNamePath.evaluate(player);
            if (name.equals(playerName)) return player;
        }
        return null;
    }


    // the item id we use to draw the ROBOTS card is special, since it depends on how the players configured 
    // robots in the options prior to game start.
    private static GuiPacketQuery robotOptiongpq = new GuiPacketQuery("options.robots");
    static public int getRobotItemId(GameEvent i_event) {
        return 70 + (Integer)robotOptiongpq.evaluate(i_event);
    }

    private static GuiPacketQuery megpq = new GuiPacketQuery("{ME}");
    public static String getMe(GameEvent i_event) {
        return (String)megpq.evaluate(i_event);
    }
}
