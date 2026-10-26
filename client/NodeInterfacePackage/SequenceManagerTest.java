package NodeInterfacePackage;

import org.json.JSONObject;
import org.json.JSONArray;

import java.util.ArrayList;
import java.util.List;

/**
 * Plain, dependency-free test runner -- see org/doghouse/jsonpath/JsonPathTest.java
 * for the project's Java test convention.
 *
 * SequenceManager has no wired-in GUI widget yet (nothing in client/TheGameMachine
 * constructs or drives one), so this is its only exercise: fake GuiPacketParsers
 * built from hand-assembled envelopes in the same shape BuildStateWalkerEnvelope/
 * HandleSequenceRequest actually produce (see DLLGame.hpp), fed straight into
 * SequenceManager.update(), with a recording SequenceCallback standing in for
 * a real widget.
 */
public class SequenceManagerTest
{
  private static int s_failures = 0;

  private static void check(String i_label, boolean i_condition)
  {
    if (i_condition)
    {
      System.out.println("ok:   " + i_label);
    }
    else
    {
      s_failures++;
      System.out.println("FAIL: " + i_label);
    }
  }

  public static void main(String[] args)
  {
    testRegisterBeforeAnyDataReplaysZero();
    testTransitionUpdateIncrementsCount();
    testUnchangedCountIsNotReannounced();
    testSequenceUpdateDeliversItemsInOrder();
    testReDeliveredItemIsNotReannounced();
    testLateRegistrationReplaysCachedStateOnlyToNewCallback();
    testErrorSequencePacketIsIgnored();
    testUnregisterStopsFurtherCallbacks();
    testSequencesAreIsolatedByName();
    testResetClearsCachedStateButKeepsCallbacksRegistered();

    System.out.println();
    System.out.println(s_failures == 0 ? "ALL PASS" : s_failures + " FAILURE(S)");
    System.exit(s_failures == 0 ? 0 : 1);
  }

  // --- a recording stand-in for a real widget's SequenceCallback ---

  private static class RecordingCallback implements SequenceManager.SequenceCallback
  {
    List<Integer> counts = new ArrayList<>();
    List<Integer> itemIndexes = new ArrayList<>();
    List<JSONObject> items = new ArrayList<>();

    public void sequenceCountIncrement(int i_newcount)
    {
      counts.add(i_newcount);
    }

    public void sequenceGetItem(int i_index, JSONObject i_content)
    {
      itemIndexes.add(i_index);
      items.add(i_content);
    }
  }

  // --- envelope builders, matching BuildStateWalkerEnvelope/HandleSequenceRequest's real shape ---

  private static GuiPacketParser transitionPacket(String i_sequenceName, int i_count)
  {
    JSONObject sequences = new JSONObject();
    sequences.put(i_sequenceName, i_count);

    JSONObject fullState = new JSONObject();
    fullState.put("state", "S");
    fullState.put("stateDesc", "D");
    fullState.put("legalActions", new JSONArray());
    fullState.put("gameInstanceId", "00000000-0000-0000-0000-000000000000");
    fullState.put("type", "transition");
    fullState.put("sequences", sequences);

    return gameGuiPacket(fullState);
  }

  private static GuiPacketParser sequencePacket(String i_sequenceName, int i_firstIndex, JSONObject... i_records)
  {
    JSONArray records = new JSONArray();
    for (JSONObject record : i_records)
    {
      records.put(record);
    }

    JSONObject body = new JSONObject();
    body.put("sequence", i_sequenceName);
    body.put("firstIndex", i_firstIndex);
    body.put("records", records);

    JSONObject fullState = new JSONObject();
    fullState.put("state", "S");
    fullState.put("stateDesc", "D");
    fullState.put("legalActions", new JSONArray());
    fullState.put("gameInstanceId", "00000000-0000-0000-0000-000000000000");
    fullState.put("type", "sequence");
    fullState.put("sequences", new JSONObject());
    fullState.put("body", body);

    return gameGuiPacket(fullState);
  }

  private static GuiPacketParser sequenceErrorPacket(String i_errorMessage)
  {
    JSONObject fullState = new JSONObject();
    fullState.put("state", "S");
    fullState.put("stateDesc", "D");
    fullState.put("legalActions", new JSONArray());
    fullState.put("gameInstanceId", "00000000-0000-0000-0000-000000000000");
    fullState.put("type", "sequence");
    fullState.put("sequences", new JSONObject());
    fullState.put("error", i_errorMessage);
    // deliberately no "body" -- HandleSequenceRequest never sets one on the error path.

    return gameGuiPacket(fullState);
  }

  private static GuiPacketParser gameGuiPacket(JSONObject i_fullState)
  {
    JSONObject envelope = new JSONObject();
    envelope.put("gui", "Outpost");
    envelope.put("recipient", "alice");
    envelope.put("XMLLoc", "loc");
    envelope.put("XMLFile", "file.xml");
    envelope.put("fullState", i_fullState);
    return new GuiPacketParser(envelope.toString());
  }

  // --- tests ---

  private static void testRegisterBeforeAnyDataReplaysZero()
  {
    SequenceManager sm = new SequenceManager();
    RecordingCallback cb = new RecordingCallback();

    sm.register(cb, "Spends");

    check("registering before any data arrives replays a count of 0",
        cb.counts.size() == 1 && cb.counts.get(0) == 0);
    check("registering before any data arrives replays no items",
        cb.items.isEmpty());
  }

  private static void testTransitionUpdateIncrementsCount()
  {
    SequenceManager sm = new SequenceManager();
    RecordingCallback cb = new RecordingCallback();
    sm.register(cb, "Spends");

    sm.update(transitionPacket("Spends", 2));

    check("a transition packet's sequences{} bumps a registered callback's count",
        cb.counts.size() == 2 && cb.counts.get(1) == 2);
  }

  private static void testUnchangedCountIsNotReannounced()
  {
    SequenceManager sm = new SequenceManager();
    RecordingCallback cb = new RecordingCallback();
    sm.register(cb, "Spends");

    sm.update(transitionPacket("Spends", 2));
    sm.update(transitionPacket("Spends", 2));

    check("delivering the same count twice only announces the increment once",
        cb.counts.size() == 2);
  }

  private static void testSequenceUpdateDeliversItemsInOrder()
  {
    SequenceManager sm = new SequenceManager();
    RecordingCallback cb = new RecordingCallback();
    sm.register(cb, "Spends");

    JSONObject item0 = new JSONObject().put("a", 1);
    JSONObject item1 = new JSONObject().put("a", 2);
    sm.update(sequencePacket("Spends", 0, item0, item1));

    check("a sequence packet delivers one item per record",
        cb.itemIndexes.size() == 2);
    check("items are indexed starting at the packet's firstIndex",
        cb.itemIndexes.get(0) == 0 && cb.itemIndexes.get(1) == 1);
    check("items are delivered in the order they arrived",
        cb.items.get(0).getInt("a") == 1 && cb.items.get(1).getInt("a") == 2);
  }

  private static void testReDeliveredItemIsNotReannounced()
  {
    SequenceManager sm = new SequenceManager();
    RecordingCallback cb = new RecordingCallback();
    sm.register(cb, "Spends");

    JSONObject item0 = new JSONObject().put("a", 1);
    sm.update(sequencePacket("Spends", 0, item0));
    sm.update(sequencePacket("Spends", 0, item0));

    check("re-delivering the same index doesn't re-announce it (append-only invariant)",
        cb.itemIndexes.size() == 1);
  }

  private static void testLateRegistrationReplaysCachedStateOnlyToNewCallback()
  {
    SequenceManager sm = new SequenceManager();
    RecordingCallback early = new RecordingCallback();
    sm.register(early, "Spends");

    sm.update(transitionPacket("Spends", 1));
    JSONObject item0 = new JSONObject().put("a", 1);
    sm.update(sequencePacket("Spends", 0, item0));

    int earlyCountAnnouncementsBeforeLateRegistration = early.counts.size();
    int earlyItemAnnouncementsBeforeLateRegistration = early.itemIndexes.size();

    RecordingCallback late = new RecordingCallback();
    sm.register(late, "Spends");

    check("a callback registering after data already arrived is replayed the current count",
        late.counts.size() == 1 && late.counts.get(0) == 1);
    check("a callback registering after data already arrived is replayed every cached item",
        late.itemIndexes.size() == 1 && late.itemIndexes.get(0) == 0);
    check("a late registration doesn't re-notify callbacks that were already registered",
        early.counts.size() == earlyCountAnnouncementsBeforeLateRegistration
            && early.itemIndexes.size() == earlyItemAnnouncementsBeforeLateRegistration);
  }

  private static void testErrorSequencePacketIsIgnored()
  {
    SequenceManager sm = new SequenceManager();
    RecordingCallback cb = new RecordingCallback();
    sm.register(cb, "Spends");

    int countAnnouncementsBefore = cb.counts.size();
    sm.update(sequenceErrorPacket("Illegal range for sequence Spends"));

    check("a sequence-type packet with an error doesn't throw and announces nothing",
        cb.counts.size() == countAnnouncementsBefore && cb.itemIndexes.isEmpty());
  }

  private static void testUnregisterStopsFurtherCallbacks()
  {
    SequenceManager sm = new SequenceManager();
    RecordingCallback cb = new RecordingCallback();
    sm.register(cb, "Spends");
    sm.unregister(cb, "Spends");

    sm.update(transitionPacket("Spends", 5));

    check("an unregistered callback receives no further count increments",
        cb.counts.size() == 1); // just the replay from register(), nothing from the update after unregister
  }

  private static void testSequencesAreIsolatedByName()
  {
    SequenceManager sm = new SequenceManager();
    RecordingCallback spendsCb = new RecordingCallback();
    RecordingCallback otherCb = new RecordingCallback();
    sm.register(spendsCb, "Spends");
    sm.register(otherCb, "OtherSequence");

    sm.update(transitionPacket("Spends", 3));

    check("updating one sequence's count doesn't notify a callback registered to a different sequence",
        otherCb.counts.size() == 1); // just its own register() replay of 0
    check("updating one sequence's count does notify the callback registered to it",
        spendsCb.counts.get(spendsCb.counts.size() - 1) == 3);
  }

  private static void testResetClearsCachedStateButKeepsCallbacksRegistered()
  {
    SequenceManager sm = new SequenceManager();
    RecordingCallback cb = new RecordingCallback();
    sm.register(cb, "Spends");
    sm.update(transitionPacket("Spends", 2));

    sm.Reset();

    // Re-announcing the same count (2) the sequence had right before Reset()
    // should fire again -- Reset() must actually zero the internal dedupe
    // state, not just leave the widget's own last-seen value unchanged.
    sm.update(transitionPacket("Spends", 2));

    check("Reset() clears the cached count, so re-delivering the same value re-announces it "
        + "to a callback that was never re-registered (Reset() doesn't unregister callbacks)",
        cb.counts.get(cb.counts.size() - 1) == 2
            && cb.counts.size() == 3); // replay(0), pre-Reset(2), post-Reset(2) again
  }
}
