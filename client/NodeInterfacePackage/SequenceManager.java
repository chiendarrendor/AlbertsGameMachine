package NodeInterfacePackage;

import org.json.JSONObject;
import org.doghouse.jsonpath.JsonPath;
import java.util.Set;
import java.util.HashSet;
import java.util.Map;
import java.util.HashMap;
import java.util.List;

// this class is a mediator for a game gui's "sequence" objects
// it's got two kinds of interfaces:
// top-end interface:  
// * reads the "sequences" field out of each FSS and 
//   calls any subscribed callers' sequenceCountIncrement() if the count increments.
// * whenever a Sequence message arrives, it iterates over all records in the message and
//   calls any subscribed callers' sequenceGetItem() for the message

public class SequenceManager {

    public interface SequenceCallback {
        public void sequenceCountIncrement(int i_newcount);
        public void sequenceGetItem(int i_index, JSONObject i_content);
    }

    private class SequenceInfo {
        public Set<SequenceCallback> callBacks = new HashSet<>();
        public Map<Integer, JSONObject> items = new HashMap<>();
        public int curcount = 0;

        public void incrementCount(int newCount) {
            if (newCount == curcount) return;
            callBacks.forEach((cb) -> { cb.sequenceCountIncrement(newCount); });
            curcount = newCount;
        }

        public void addItem(int index, JSONObject item) {
            if (items.containsKey(index)) return;
            callBacks.forEach((cb) -> { cb.sequenceGetItem(index,item); });
            items.put(index,item);
        }

        public void addCaller(SequenceCallback scb) {
            if (callBacks.contains(scb)) return;

            scb.sequenceCountIncrement(curcount);
            items.forEach((index, item) -> { scb.sequenceGetItem(index,item); });
            callBacks.add(scb);
        }

        public void Reset() {
            curcount = 0;
            items.clear();
        }

        public void removeCaller(SequenceCallback scb) {
            callBacks.remove(scb);
        }

    }

    private Map<String,SequenceInfo> sequences = new HashMap<>();
    private ActionSender m_actionSender;

    private SequenceInfo gocSequence(String name) {
        if (!sequences.containsKey(name)) sequences.put(name,new SequenceInfo());
        return sequences.get(name);
    }


    public SequenceManager(ActionSender i_ActionSender) {
        m_actionSender = i_ActionSender;
    }

    private  void updateSequenceCounts(GuiPacketParser i_gpp) {
        i_gpp.getSequenceMap().forEach((key, value) -> { gocSequence(key).incrementCount(value); });
    }

    private static JsonPath sequenceNameQuery = JsonPath.compile("sequence");
    private static JsonPath firstIndexQuery = JsonPath.compile("firstIndex");
    private static JsonPath itemsQuery = JsonPath.compile("records[]");

    @SuppressWarnings("unchecked")
    private void updateSequenceItems(GuiPacketParser i_gpp) {
        // TODO: I don't know what to do yet with a bad Sequence Packet...
        if (i_gpp.getErrorMessage() != null) return;

        JSONObject body = i_gpp.getBody();
        String sequenceName = (String)sequenceNameQuery.evaluate(body);
        int firstIndex = (Integer)firstIndexQuery.evaluate(body);
        List<JSONObject> items = (List<JSONObject>)itemsQuery.evaluate(body);

        int curIndex = firstIndex;
        SequenceInfo si = gocSequence(sequenceName);
        for (JSONObject item : items ) {
            si.addItem(curIndex++,item);
        }
    }

    // assumes that this is some sort of game gui packet (not a LOGIN)
    public void update(GuiPacketParser i_gpp) {
        switch(i_gpp.getActionContentType()) {
            case ActionContentType.TRANSITION: updateSequenceCounts(i_gpp); return;
            case ActionContentType.SEQUENCE: updateSequenceItems(i_gpp); return;
            default:
                // TODO again don't know what do do here yet.
        }
    }

    public void register(SequenceCallback scb, String sequenceName) {
        gocSequence(sequenceName).addCaller(scb);
    }

    public void unregister(SequenceCallback scb, String sequenceName) {
        gocSequence(sequenceName).removeCaller(scb);
    }

    public void Reset() {
        sequences.forEach((key, value) -> { value.Reset(); });
    }

    public void requestItems(String sequenceName, int i_start, int i_end) {
        // GameServerMain.hpp's "sequence" dispatch: the action name doubles as
        // the sequence name, and "start"/"end" (the [start, end) range) are the
        // only params it reads -- see HandleSequenceRequest in DLLGame.hpp.
        JSONObject body = new JSONObject();
        body.put("start", i_start);
        body.put("end", i_end);

        m_actionSender.SendAction(sequenceName, body, ActionContentType.SEQUENCE);
    }
}