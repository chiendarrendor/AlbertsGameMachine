package org.doghouse.jsonpath;

import org.json.JSONObject;

import java.util.List;

/**
 * Plain, dependency-free test runner -- no JUnit or similar is vendored
 * anywhere in this project's Java client, so this project's convention is a
 * main()-driven class with simple pass/fail checks, same spirit as the C++
 * side's Boost tests without pulling in a framework. Exits non-zero if any
 * check fails, for use from a Makefile `test` target.
 *
 * Lives alongside JsonPath itself (same package, same directory) rather
 * than a separate top-level test tree -- the project's convention is tests
 * for a top-level client/ directory live in that directory.
 */
public class JsonPathTest
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
    JSONObject body = new JSONObject(
        "{"
        + "\"moveMediator\": {\"currentDest\": \"Mars\"},"
        + "\"megaRequest\": null,"
        + "\"qboxes\": {\"Great Hall\": {\"contents\": \"stuff\"}, \"Venus\": {\"contents\": null}},"
        + "\"rooms\": [{\"name\": \"Great Hall\"}, {\"name\": \"GameRoom\"}]"
        + "}");

    testPlainFieldAccess(body);
    testFinalNullIsSuccess();
    testNavigateThroughNullFailsAndDefaults(body);
    testSizeOperator(body);
    testMapOperator(body);
    testIndexOutOfBounds(body);
    testQuotedKeyLookup(body);
    testAbsentKeyFails(body);
    testTypeMismatchFails(body);
    testNoRootVocabulary(body);
    testSyntaxErrors();

    System.out.println();
    System.out.println(s_failures == 0 ? "ALL PASS" : s_failures + " FAILURE(S)");
    System.exit(s_failures == 0 ? 0 : 1);
  }

  private static void testPlainFieldAccess(JSONObject body)
  {
    check("nested field access", "Mars".equals(JsonPath.compile("moveMediator.currentDest").evaluate(body)));
  }

  private static void testFinalNullIsSuccess()
  {
    JSONObject bodyWithNullDest = new JSONObject("{\"moveMediator\": {\"currentDest\": null}}");
    check("a path's final value being JSON null is a successful result, not an exception",
        JsonPath.compile("moveMediator.currentDest").evaluate(bodyWithNullDest) == null);
  }

  private static void testNavigateThroughNullFailsAndDefaults(JSONObject body)
  {
    JsonPath megaMax = JsonPath.compile("megaRequest.maxMegas");

    boolean threw = false;
    try
    {
      megaMax.evaluate(body);
    }
    catch (JsonQueryException e)
    {
      threw = true;
      check("navigate-through-null reports the right reason", e.getReason() == JsonQueryException.Reason.NULL_ENCOUNTERED);
    }
    check("navigating through a null throws by default", threw);
    check("evaluate(root, default) returns the default instead of throwing",
        Integer.valueOf(0).equals(megaMax.evaluate(body, 0)));
    check("evaluateInt(root, default) returns the default on a null-mid-chain", megaMax.evaluateInt(body, 0) == 0);
  }

  private static void testSizeOperator(JSONObject body)
  {
    check("#size counts keys of a dictionary-style object",
        Integer.valueOf(2).equals(JsonPath.compile("qboxes#size").evaluate(body)));
  }

  private static void testMapOperator(JSONObject body)
  {
    List<String> names = JsonPath.compile("rooms[].name").evaluateStringList(body);
    check("[] projects a field across every array element",
        names.size() == 2 && names.get(0).equals("Great Hall") && names.get(1).equals("GameRoom"));
  }

  private static void testIndexOutOfBounds(JSONObject body)
  {
    boolean threw = false;
    try
    {
      JsonPath.compile("rooms[5]").evaluate(body);
    }
    catch (JsonQueryException e)
    {
      threw = true;
      check("out-of-bounds index reports the right reason", e.getReason() == JsonQueryException.Reason.INDEX_OUT_OF_BOUNDS);
    }
    check("an out-of-bounds array index throws", threw);
  }

  private static void testQuotedKeyLookup(JSONObject body)
  {
    check("a quoted key does a dictionary-style lookup by literal name",
        "stuff".equals(JsonPath.compile("qboxes[\"Great Hall\"].contents").evaluate(body)));
    check("a quoted-key lookup resolving to a final null is also a success, not an exception",
        JsonPath.compile("qboxes[\"Venus\"].contents").evaluate(body) == null);
  }

  private static void testAbsentKeyFails(JSONObject body)
  {
    boolean threw = false;
    try
    {
      JsonPath.compile("nope").evaluate(body);
    }
    catch (JsonQueryException e)
    {
      threw = true;
      check("a genuinely absent key reports the right reason", e.getReason() == JsonQueryException.Reason.KEY_ABSENT);
    }
    check("a genuinely absent key throws", threw);
  }

  private static void testTypeMismatchFails(JSONObject body)
  {
    boolean threw = false;
    try
    {
      JsonPath.compile("moveMediator.currentDest.nope").evaluate(body);
    }
    catch (JsonQueryException e)
    {
      threw = true;
      check("navigating a field access into a scalar reports the right reason",
          e.getReason() == JsonQueryException.Reason.TYPE_MISMATCH);
    }
    check("navigating a field access into a scalar throws", threw);
  }

  private static void testNoRootVocabulary(JSONObject body)
  {
    // JsonPath has no special-cased "root" names (no "body"/"me"/"state"/...)
    // -- any identifier is a legal first segment, evaluated against whatever
    // object the caller passes to evaluate().
    Object v = JsonPath.compile("moveMediator").evaluate(body);
    check("the first segment is ordinary field access, not a distinguished root",
        v instanceof JSONObject && "Mars".equals(((JSONObject) v).getString("currentDest")));
  }

  private static void testSyntaxErrors()
  {
    check("content after a '[]' map must start a new segment", throwsSyntaxError("rooms[]foo"));
    check("'#size' must be the last segment of a path", throwsSyntaxError("rooms#size.foo"));
    check("an unbalanced bracket is a syntax error", throwsSyntaxError("rooms[1"));
    check("an empty path is a syntax error", throwsSyntaxError(""));
  }

  private static boolean throwsSyntaxError(String i_path)
  {
    try
    {
      JsonPath.compile(i_path);
      return false;
    }
    catch (JsonPathSyntaxException e)
    {
      return true;
    }
  }
}
