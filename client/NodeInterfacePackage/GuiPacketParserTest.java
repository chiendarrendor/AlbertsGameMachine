package NodeInterfacePackage;

import org.json.JSONException;
import org.doghouse.jsonpath.JsonQueryException;

/**
 * Plain, dependency-free test runner -- see org/doghouse/jsonpath/JsonPathTest.java
 * for the project's Java test convention.
 *
 * Written *before* the in-progress JsonQuery-based internal rewrite of
 * GuiPacketParser, specifically so that rewrite can be checked against it --
 * a correct rewrite should leave every one of these passing unchanged.
 * These tests pin down GuiPacketParser's current observable behavior, not
 * what it "should" be -- found and removed one real dead-code case this way
 * already (isErrorMessage()/m_isError, 2026-10-18: never assigned anywhere,
 * no real caller anywhere in the client, deleted outright rather than
 * pinned, since ClientManager and everything downstream of it already only
 * ever checks getErrorMessage() != null).
 *
 * getMyRoomName()/getMyRoomGame() and their tests were removed outright
 * (2026-10-18), not pinned -- ROOMGUI's "myroom" full-state field (see
 * .claude/server_game_interface_spec.md) makes them redundant, and
 * ClientManager (their only real caller) has already been updated to query
 * "myroom" directly instead.
 */
public class GuiPacketParserTest
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
    testLoginWithError();
    testLoginWithoutError();
    testLoginWithExplicitNullError();

    testRoomGuiSuccess();
    testRoomGuiError();

    testGameGuiSuccess();
    testGameGuiError();

    testMissingGuiFieldThrows();
    testExplicitNullGuiFieldThrows();
    testMalformedJsonThrows();
    testMissingRequiredFieldThrows();

    System.out.println();
    System.out.println(s_failures == 0 ? "ALL PASS" : s_failures + " FAILURE(S)");
    System.exit(s_failures == 0 ? 0 : 1);
  }

  // --- LOGIN packets ---

  private static void testLoginWithError()
  {
    GuiPacketParser gpp = new GuiPacketParser(
        "{\"gui\":\"LOGIN\",\"connectstring\":\"Welcome!\",\"authenticated\":false,"
        + "\"otherconnections\":-1,\"error\":\"bad password\"}");

    check("LOGIN gui type", gpp.getGuiType() == GuiPacketParser.GuiType.LOGIN);
    check("LOGIN connect string", "Welcome!".equals(gpp.getLoginConnectString()));
    check("LOGIN authenticated false", !gpp.isLoginAuthenticated());
    check("LOGIN other connections", gpp.getLoginOtherConnectionCount() == -1);
    check("LOGIN error present", "bad password".equals(gpp.getLoginError()));
  }

  private static void testLoginWithoutError()
  {
    GuiPacketParser gpp = new GuiPacketParser(
        "{\"gui\":\"LOGIN\",\"connectstring\":\"Welcome!\",\"authenticated\":true,\"otherconnections\":0}");

    check("LOGIN authenticated true", gpp.isLoginAuthenticated());
    check("LOGIN other connections zero", gpp.getLoginOtherConnectionCount() == 0);
    check("LOGIN error absent entirely -> null, not an exception", gpp.getLoginError() == null);
  }

  private static void testLoginWithExplicitNullError()
  {
    GuiPacketParser gpp = new GuiPacketParser(
        "{\"gui\":\"LOGIN\",\"connectstring\":\"Welcome!\",\"authenticated\":true,"
        + "\"otherconnections\":0,\"error\":null}");

    check("LOGIN error explicitly JSON null -> null, same as absent", gpp.getLoginError() == null);
  }

  // --- ROOMGUI packets ---

  private static final String ROOMGUI_SUCCESS =
      "{\"gui\":\"ROOMGUI\",\"recipient\":\"alice\",\"XMLLoc\":\"loc\",\"XMLFile\":\"file.xml\","
      + "\"fullState\":{\"state\":\"ROOM\",\"stateDesc\":\"Room Navigation\","
      + "\"legalActions\":[\"NEWROOM\",\"CHANGEROOM\"],"
      + "\"gameInstanceId\":\"00000000-0000-0000-0000-000000000000\","
      + "\"body\":{"
      + "\"rooms\":["
      +   "{\"name\":\"Great Hall\",\"gameName\":\"\",\"isDone\":true,\"gameTypes\":[\"Outpost\"],\"savedGames\":[]},"
      +   "{\"name\":\"GameRoom\",\"gameName\":\"Outpost\",\"isDone\":false,\"gameTypes\":[],\"savedGames\":[\"save1\"]}"
      + "],"
      + "\"inhabitants\":[{\"name\":\"alice\",\"room\":\"GameRoom\"},{\"name\":\"bob\",\"room\":\"Great Hall\"}],"
      + "\"myroom\":{\"name\":\"GameRoom\",\"gameName\":\"Outpost\",\"isDone\":false,\"gameTypes\":[],\"savedGames\":[\"save1\"]}"
      + "}}}";

  private static void testRoomGuiSuccess()
  {
    GuiPacketParser gpp = new GuiPacketParser(ROOMGUI_SUCCESS);

    check("ROOMGUI gui type", gpp.getGuiType() == GuiPacketParser.GuiType.ROOMGUI);
    check("ROOMGUI game name is the literal 'ROOMGUI'", "ROOMGUI".equals(gpp.getGameName()));
    check("ROOMGUI recipient", "alice".equals(gpp.getMyName()));
    check("ROOMGUI XMLLoc", "loc".equals(gpp.getXMLLoc()));
    check("ROOMGUI XMLFile", "file.xml".equals(gpp.getXMLFile()));
    check("ROOMGUI state name", "ROOM".equals(gpp.getStateName()));
    check("ROOMGUI state desc", "Room Navigation".equals(gpp.getStateDesc()));
    check("ROOMGUI legal action present", gpp.hasLegalAction("NEWROOM"));
    check("ROOMGUI legal action absent", !gpp.hasLegalAction("NEWGAME"));
    check("ROOMGUI no error", gpp.getErrorMessage() == null);
    check("ROOMGUI body present", gpp.getBody() != null);
    // ROOMGUI isn't a real "game instance" (it's a fixed single-state FSM
    // shared across every room, not reloadable from a save file), so the
    // server sends a fixed nil UUID here rather than omitting the field --
    // keeps every envelope's shape identical regardless of gui type. See
    // RoomManager.cpp's ROOMGUI_INSTANCE_ID / StateWalker::GetInstanceId.
    check("ROOMGUI gameInstanceId is the fixed nil UUID",
        "00000000-0000-0000-0000-000000000000".equals(gpp.getUUID()));
  }

  private static void testRoomGuiError()
  {
    GuiPacketParser gpp = new GuiPacketParser(
        "{\"gui\":\"ROOMGUI\",\"recipient\":\"alice\",\"XMLLoc\":\"loc\",\"XMLFile\":\"file.xml\","
        + "\"fullState\":{\"state\":\"ROOM\",\"stateDesc\":\"Room Navigation\","
        + "\"legalActions\":[\"NEWROOM\",\"CHANGEROOM\"],"
        + "\"gameInstanceId\":\"00000000-0000-0000-0000-000000000000\","
        + "\"error\":\"Don't know which room you mean\"}}");

    check("ROOMGUI error message", "Don't know which room you mean".equals(gpp.getErrorMessage()));
    check("ROOMGUI body absent on error", gpp.getBody() == null);
  }

  // --- GAMEGUI packets ---

  private static final String GAMEGUI_SUCCESS =
      "{\"gui\":\"Outpost\",\"recipient\":\"alice\",\"XMLLoc\":\"loc\",\"XMLFile\":\"file.xml\","
      + "\"fullState\":{\"state\":\"InitialState\",\"stateDesc\":\"Setting up\","
      + "\"legalActions\":[\"JOIN\",\"VALIDATESETUP\"],"
      + "\"gameInstanceId\":\"11111111-1111-1111-1111-111111111111\","
      + "\"body\":{\"myPlayerId\":0,\"options\":{}}}}";

  private static void testGameGuiSuccess()
  {
    GuiPacketParser gpp = new GuiPacketParser(GAMEGUI_SUCCESS);

    check("GAMEGUI gui type", gpp.getGuiType() == GuiPacketParser.GuiType.GAMEGUI);
    check("GAMEGUI game name is the real game's name", "Outpost".equals(gpp.getGameName()));
    check("GAMEGUI state name", "InitialState".equals(gpp.getStateName()));
    check("GAMEGUI legal action present", gpp.hasLegalAction("JOIN"));
    check("GAMEGUI legal action absent", !gpp.hasLegalAction("DONE"));
    check("GAMEGUI body present", gpp.getBody() != null);
    check("GAMEGUI body field readable", gpp.getBody().getInt("myPlayerId") == 0);
    // Unlike ROOMGUI's fixed nil, a real game's gameInstanceId is a genuine
    // per-instance random UUID from StateWalker::GetInstanceId -- this test
    // doesn't need the actual generation behavior (that's server-side/C++
    // tested), just that GuiPacketParser reads whatever value is there.
    check("GAMEGUI gameInstanceId", "11111111-1111-1111-1111-111111111111".equals(gpp.getUUID()));
  }

  private static void testGameGuiError()
  {
    GuiPacketParser gpp = new GuiPacketParser(
        "{\"gui\":\"Outpost\",\"recipient\":\"alice\",\"XMLLoc\":\"loc\",\"XMLFile\":\"file.xml\","
        + "\"fullState\":{\"state\":\"InitialState\",\"stateDesc\":\"Setting up\","
        + "\"legalActions\":[\"JOIN\"],"
        + "\"gameInstanceId\":\"11111111-1111-1111-1111-111111111111\","
        + "\"error\":\"That is not a legal move.\"}}");

    check("GAMEGUI error message", "That is not a legal move.".equals(gpp.getErrorMessage()));
    check("GAMEGUI body absent on error", gpp.getBody() == null);
  }

  // --- malformed input ---

  private static void testMissingGuiFieldThrows()
  {
    // "gui" is a required field, fetched via JsonPath's one-arg, throwing
    // evaluate() -- same convention as every other required field -- so an
    // entirely absent key throws JsonQueryException, not JSONException.
    check("a packet with no 'gui' field at all throws JsonQueryException",
        throwsException(JsonQueryException.class, () -> new GuiPacketParser("{\"recipient\":\"alice\"}")));
  }

  private static void testExplicitNullGuiFieldThrows()
  {
    // A genuinely odd, worth-noting asymmetry: a path's final value being
    // JSON null is a *successful* resolution under JsonPath's semantics
    // (null is a legitimate answer, not a failure -- see JsonPathTest), so
    // evaluate() happily returns null here rather than throwing. The
    // constructor's own explicit "if (gui == null) throw new
    // JSONException(...)" check is what actually catches this case -- so
    // "gui" entirely absent and "gui" explicitly null currently throw two
    // different exception types for what's conceptually the same "packet
    // has no usable gui field" problem. Harmless in practice (nothing
    // downstream catches either type specifically -- ClientManager catches
    // plain Exception around GuiPacketParser's construction), but pinned
    // here so it's a documented, deliberate quirk rather than a surprise.
    check("a packet with an explicit 'gui': null throws JSONException instead",
        throwsException(JSONException.class,
            () -> new GuiPacketParser("{\"gui\":null,\"recipient\":\"alice\"}")));
  }

  private static void testMalformedJsonThrows()
  {
    // Fails at `new JSONObject(i_string)` itself, before any JsonPath
    // evaluation is reached -- untouched by the rewrite, still JSONException.
    check("non-JSON input throws JSONException",
        throwsException(JSONException.class, () -> new GuiPacketParser("not json at all")));
  }

  private static void testMissingRequiredFieldThrows()
  {
    // Unlike the login-error/fullState-error/fullState-body fields (which
    // are deliberately optional), "state" has no such guard -- a packet
    // missing it is expected to throw today, not silently produce a null
    // getStateName(). Worth pinning explicitly, since a careless
    // JsonPath-based rewrite could easily soften this into silent
    // null-defaulting without anyone noticing (and in fact did, briefly,
    // during this exact rewrite). Now that "state" is fetched via the
    // one-arg, throwing evaluate(), the exception type is JsonQueryException,
    // not the original JSONException.
    check("a ROOMGUI/GAMEGUI packet missing the required 'state' field throws",
        throwsException(JsonQueryException.class, () -> new GuiPacketParser(
            "{\"gui\":\"Outpost\",\"recipient\":\"alice\",\"XMLLoc\":\"loc\",\"XMLFile\":\"file.xml\","
            + "\"fullState\":{\"stateDesc\":\"Setting up\",\"legalActions\":[]}}")));
  }

  private interface ThrowingRunnable
  {
    void run();
  }

  private static boolean throwsException(Class<? extends Throwable> i_expected, ThrowingRunnable i_action)
  {
    try
    {
      i_action.run();
      return false;
    }
    catch (Throwable e)
    {
      return i_expected.isInstance(e);
    }
  }
}
