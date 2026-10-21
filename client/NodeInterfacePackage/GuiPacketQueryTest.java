package NodeInterfacePackage;

import java.util.List;

/**
 * Plain, dependency-free test runner -- see org/doghouse/jsonpath/JsonPathTest.java
 * for the project's Java test convention (no framework vendored, a main()
 * with simple pass/fail checks, non-zero exit on failure).
 */
public class GuiPacketQueryTest
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

  private static GameEvent sampleEvent()
  {
    String packet = "{\"gui\":\"Outpost\",\"recipient\":\"alice\",\"XMLLoc\":\"x\",\"XMLFile\":\"y\","
        + "\"fullState\":{\"state\":\"S\",\"stateDesc\":\"D\",\"legalActions\":[\"DONE\"],"
        + "\"gameInstanceId\":\"00000000-0000-0000-0000-000000000000\","
        + "\"body\":{\"status\":\"RealStringValue\",\"names\":[{\"n\":\"a\"},{\"n\":\"b\"}],"
        + "\"count\":7,\"flag\":true}}}";
    return new GameEvent(new GuiPacketParser(packet));
  }

  public static void main(String[] args)
  {
    GameEvent event = sampleEvent();

    testPlainQueries(event);
    testSentinels(event);
    testTypeMismatchFallsBackToDefault(event);
    testTypeGuardRejectsWrongAccessor(event);
    testEventWithNoGppIsRejected();

    System.out.println();
    System.out.println(s_failures == 0 ? "ALL PASS" : s_failures + " FAILURE(S)");
    System.exit(s_failures == 0 ? 0 : 1);
  }

  private static void testPlainQueries(GameEvent event)
  {
    // Anything that isn't one of the four {...} sentinels is a plain
    // org.doghouse.jsonpath expression evaluated against the event's
    // GuiPacketParser's getBody().
    check("plain query -> evaluateString reads from the body",
        "RealStringValue".equals(new GuiPacketQuery("status").evaluateString(event, "DEFAULT")));
    check("plain query -> evaluateInt reads from the body",
        new GuiPacketQuery("count").evaluateInt(event, -1) == 7);
    check("plain query -> evaluateBoolean reads from the body",
        new GuiPacketQuery("flag").evaluateBoolean(event, false));

    List<String> names = new GuiPacketQuery("names[].n").evaluateStringList(event);
    check("plain query -> evaluateStringList reads from the body",
        names.size() == 2 && names.get(0).equals("a") && names.get(1).equals("b"));
  }

  private static void testSentinels(GameEvent event)
  {
    check("{STATE} resolves to the packet's state name",
        "S".equals(new GuiPacketQuery("{STATE}").evaluateString(event, "DEFAULT")));
    check("{STATEDESC} resolves to the packet's state description",
        "D".equals(new GuiPacketQuery("{STATEDESC}").evaluateString(event, "DEFAULT")));
    check("{ME} resolves to the packet's recipient",
        "alice".equals(new GuiPacketQuery("{ME}").evaluateString(event, "DEFAULT")));
    check("{LEGALACTION:name} is true for a name in legalActions",
        new GuiPacketQuery("{LEGALACTION:DONE}").evaluateBoolean(event, false));
    check("{LEGALACTION:name} is false for a name not in legalActions",
        !new GuiPacketQuery("{LEGALACTION:NOPE}").evaluateBoolean(event, true));
  }

  private static void testTypeMismatchFallsBackToDefault(GameEvent event)
  {
    // GuiPacketQuery's own type guard lets this through (a plain query's
    // RetType is QRET_ANY, so every accessor is nominally legal) -- the
    // mismatch is only caught once it reaches org.doghouse.jsonpath itself,
    // which falls back to the default rather than throwing.
    check("evaluateInt on a string-valued plain query falls back to the default",
        new GuiPacketQuery("status").evaluateInt(event, -1) == -1);
  }

  private static void testTypeGuardRejectsWrongAccessor(GameEvent event)
  {
    // Unlike the plain-query case above, a sentinel has one single, fixed
    // RetType -- calling the wrong accessor on one is rejected by
    // GuiPacketQuery's own guard before org.doghouse.jsonpath is ever
    // involved (indeed, {STATE}/{LEGALACTION:...} never compile a JsonPath
    // at all, so there would be nothing to delegate to).
    check("evaluateBoolean on {STATE} (a string sentinel) is rejected",
        throwsNodeInterfacePackageException(() -> new GuiPacketQuery("{STATE}").evaluateBoolean(event, false)));
    check("evaluateString on {LEGALACTION:...} (a boolean sentinel) is rejected",
        throwsNodeInterfacePackageException(() -> new GuiPacketQuery("{LEGALACTION:DONE}").evaluateString(event, "x")));
  }

  private static void testEventWithNoGppIsRejected()
  {
    // GameEvent(String) is the old, dead-on-the-live-wire construction path
    // (see GUITester.java's own TODO) -- it never sets a GPP, so getGPP()
    // returns null. Every GuiPacketQuery accessor should reject that
    // cleanly rather than NPE-ing on gpp.getStateName()/etc.
    GameEvent noGppEvent = new GameEvent("SOMEOLDEVENT");
    check("evaluateString on an event with no GPP is rejected, not an NPE",
        throwsNodeInterfacePackageException(() -> new GuiPacketQuery("{ME}").evaluateString(noGppEvent, "x")));
    check("a plain query on an event with no GPP is also rejected, not an NPE",
        throwsNodeInterfacePackageException(() -> new GuiPacketQuery("status").evaluateString(noGppEvent, "x")));
  }

  private interface ThrowingRunnable
  {
    void run();
  }

  private static boolean throwsNodeInterfacePackageException(ThrowingRunnable i_action)
  {
    try
    {
      i_action.run();
      return false;
    }
    catch (NodeInterfacePackageException e)
    {
      return true;
    }
  }
}
