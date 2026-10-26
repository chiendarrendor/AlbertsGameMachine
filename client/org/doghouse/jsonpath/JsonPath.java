package org.doghouse.jsonpath;

import org.json.JSONArray;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

/**
 * A compiled, reusable query expression over org.json's JSONObject/JSONArray
 * trees -- the "Pattern" to a hand-written path string's regex, in the same
 * spirit as java.util.regex.Pattern/Matcher. Depends on nothing but org.json;
 * has no knowledge of any particular application's JSON shape.
 *
 * Compile once, evaluate many times:
 * <pre>
 *   JsonPath p = JsonPath.compile("moveMediator.currentDest");
 *   Object v = p.evaluate(body);              // throws on failure
 *   Object v = p.evaluate(body, null);         // returns null on failure instead
 * </pre>
 *
 * Grammar: a path is a chain of segments -- '.field', '["field"]' (identical
 * meaning, just a different spelling for keys that aren't valid bare
 * identifiers), '[N]' (array index), '[]' (map: apply the rest of the path to
 * every element of an array, collecting the results into a List), '{}' (dict:
 * apply the rest of the path to every *value* of an object, collecting the
 * results into a Map<String,Object> keyed by the object's own field names --
 * for an object used as an arbitrary-keys lookup table rather than a
 * fixed-shape struct, e.g. a game's per-viewer FSS "sequences" field), and a
 * terminal '#size' (element count of an array, or key count of an object).
 * There is no distinguished "root" segment -- evaluate(root) simply starts
 * walking from whatever value is passed in.
 *
 * Missing-value semantics: a path's *final* value being JSON null is a
 * normal, successful result (mirrors how both game schemas use explicit
 * "type": ["X","null"] for "not applicable right now"). Needing to navigate
 * *through* a null, a genuinely absent key, an out-of-bounds index, or an
 * operator applied to the wrong kind of value are all failures -- by
 * default they throw JsonQueryException, or return a caller-supplied
 * default instead if one was given. A failure inside a '[]' map is a
 * failure of the whole expression, not a partial result.
 */
public final class JsonPath
{
  private final String m_source;
  private final List<Step> m_steps;

  private JsonPath(String i_source, List<Step> i_steps)
  {
    m_source = i_source;
    m_steps = i_steps;
  }

  /** @throws JsonPathSyntaxException if the path string is malformed. */
  public static JsonPath compile(String i_path)
  {
    return new JsonPath(i_path, PathParser.parse(i_path));
  }

  public String getSource()
  {
    return m_source;
  }

  @Override
  public String toString()
  {
    return m_source;
  }

  /** @throws JsonQueryException if the path doesn't resolve against i_root. */
  public Object evaluate(Object i_root)
  {
    Object result = evaluateSteps(m_steps, i_root);
    return unwrapNull(result);
  }

  /** Never throws JsonQueryException -- returns i_defaultValue on any failure. */
  public Object evaluate(Object i_root, Object i_defaultValue)
  {
    try
    {
      return evaluate(i_root);
    }
    catch (JsonQueryException e)
    {
      return i_defaultValue;
    }
  }

  /**
   * A legitimately-resolved null is returned as null, not i_defaultValue --
   * only an actual failure (missing/type-mismatch/...) falls back to the
   * default. See the class-level note on missing-value semantics.
   */
  public String evaluateString(Object i_root, String i_defaultValue)
  {
    try
    {
      Object v = evaluate(i_root);
      if (v == null)
      {
        return null;
      }
      if (v instanceof String s)
      {
        return s;
      }
      throw typeMismatch("String", v);
    }
    catch (JsonQueryException e)
    {
      return i_defaultValue;
    }
  }

  /**
   * int can't represent a legitimately-resolved null, so unlike
   * evaluateString, null and failure are indistinguishable here -- both
   * fall back to i_defaultValue.
   */
  public int evaluateInt(Object i_root, int i_defaultValue)
  {
    try
    {
      Object v = evaluate(i_root);
      if (v == null)
      {
        return i_defaultValue;
      }
      if (v instanceof Number n)
      {
        return n.intValue();
      }
      throw typeMismatch("int", v);
    }
    catch (JsonQueryException e)
    {
      return i_defaultValue;
    }
  }

  /** Same null-can't-be-represented reasoning as evaluateInt. */
  public boolean evaluateBoolean(Object i_root, boolean i_defaultValue)
  {
    try
    {
      Object v = evaluate(i_root);
      if (v == null)
      {
        return i_defaultValue;
      }
      if (v instanceof Boolean b)
      {
        return b;
      }
      throw typeMismatch("boolean", v);
    }
    catch (JsonQueryException e)
    {
      return i_defaultValue;
    }
  }

  /** @throws JsonQueryException if the path fails, or resolves to something
   *  other than a '[]'-produced list of strings. Null is returned as null. */
  public List<String> evaluateStringList(Object i_root)
  {
    Object v = evaluate(i_root);
    if (v == null)
    {
      return null;
    }
    if (!(v instanceof List<?> list))
    {
      throw typeMismatch("array (via '[]')", v);
    }
    List<String> result = new ArrayList<String>();
    for (Object element : list)
    {
      if (!(element instanceof String s))
      {
        throw typeMismatch("String element", element);
      }
      result.add(s);
    }
    return result;
  }

  public List<String> evaluateStringList(Object i_root, List<String> i_defaultValue)
  {
    try
    {
      List<String> result = evaluateStringList(i_root);
      return (result == null) ? i_defaultValue : result;
    }
    catch (JsonQueryException e)
    {
      return i_defaultValue;
    }
  }

  // --- evaluation of the compiled step list ---

  private Object evaluateSteps(List<Step> i_steps, Object i_root)
  {
    Object current = i_root;
    for (int i = 0; i < i_steps.size(); i++)
    {
      Step step = i_steps.get(i);
      if (step instanceof FieldStep f)
      {
        current = stepIntoField(current, f.name(), i);
      }
      else if (step instanceof IndexStep ix)
      {
        current = stepIntoIndex(current, ix.index(), i);
      }
      else if (step instanceof MapStep m)
      {
        return stepMap(current, m.subpath(), i); // map always consumes the rest
      }
      else if (step instanceof DictStep d)
      {
        return stepDict(current, d.subpath(), i); // dict always consumes the rest
      }
      else if (step instanceof SizeStep)
      {
        return stepSize(current, i); // #size is always last
      }
    }
    return current;
  }

  private Object stepIntoField(Object i_current, String i_name, int i_at)
  {
    requireNavigable(i_current, i_at);
    if (!(i_current instanceof JSONObject obj))
    {
      throw fail(i_at, JsonQueryException.Reason.TYPE_MISMATCH,
          "expected an object to read field '" + i_name + "' from, found " + describe(i_current));
    }
    if (!obj.has(i_name))
    {
      throw fail(i_at, JsonQueryException.Reason.KEY_ABSENT, "no field named '" + i_name + "'");
    }
    return obj.get(i_name);
  }

  private Object stepIntoIndex(Object i_current, int i_index, int i_at)
  {
    requireNavigable(i_current, i_at);
    if (!(i_current instanceof JSONArray arr))
    {
      throw fail(i_at, JsonQueryException.Reason.TYPE_MISMATCH,
          "expected an array to index with [" + i_index + "], found " + describe(i_current));
    }
    if (i_index < 0 || i_index >= arr.length())
    {
      throw fail(i_at, JsonQueryException.Reason.INDEX_OUT_OF_BOUNDS,
          "index " + i_index + " out of bounds for array of length " + arr.length());
    }
    return arr.get(i_index);
  }

  private Object stepMap(Object i_current, List<Step> i_subpath, int i_at)
  {
    requireNavigable(i_current, i_at);
    if (!(i_current instanceof JSONArray arr))
    {
      throw fail(i_at, JsonQueryException.Reason.TYPE_MISMATCH,
          "expected an array for '[]', found " + describe(i_current));
    }
    List<Object> results = new ArrayList<Object>();
    for (int j = 0; j < arr.length(); j++)
    {
      // A failure here is a failure of the whole map -- it propagates
      // straight up, not swallowed per-element. See the class-level note.
      results.add(unwrapNull(evaluateSteps(i_subpath, arr.get(j))));
    }
    return results;
  }

  private Object stepDict(Object i_current, List<Step> i_subpath, int i_at)
  {
    requireNavigable(i_current, i_at);
    if (!(i_current instanceof JSONObject obj))
    {
      throw fail(i_at, JsonQueryException.Reason.TYPE_MISMATCH,
          "expected an object for '{}', found " + describe(i_current));
    }
    // org.json's JSONObject deliberately iterates its keys in an unspecified
    // (hash-based) order, not insertion order -- LinkedHashMap here just
    // gives a stable, ordinary Map type to return, not a promise that this
    // order matches the original JSON text.
    Map<String, Object> results = new LinkedHashMap<String, Object>();
    for (String key : obj.keySet())
    {
      // A failure here is a failure of the whole dict, not swallowed
      // per-key -- same as '[]'. See the class-level note.
      results.put(key, unwrapNull(evaluateSteps(i_subpath, obj.get(key))));
    }
    return results;
  }

  private Object stepSize(Object i_current, int i_at)
  {
    requireNavigable(i_current, i_at);
    if (i_current instanceof JSONArray arr)
    {
      return arr.length();
    }
    if (i_current instanceof JSONObject obj)
    {
      return obj.length();
    }
    throw fail(i_at, JsonQueryException.Reason.TYPE_MISMATCH,
        "'#size' needs an array or object, found " + describe(i_current));
  }

  private void requireNavigable(Object i_current, int i_at)
  {
    if (i_current == null || i_current == JSONObject.NULL)
    {
      throw fail(i_at, JsonQueryException.Reason.NULL_ENCOUNTERED, "cannot navigate further, value is null");
    }
  }

  private static Object unwrapNull(Object i_value)
  {
    return (i_value == JSONObject.NULL) ? null : i_value;
  }

  private JsonQueryException typeMismatch(String i_expected, Object i_actual)
  {
    return new JsonQueryException(m_source, m_steps.size() - 1, JsonQueryException.Reason.TYPE_MISMATCH,
        "expected " + i_expected + " but found " + describe(i_actual) + " evaluating '" + m_source + "'");
  }

  private JsonQueryException fail(int i_at, JsonQueryException.Reason i_reason, String i_message)
  {
    return new JsonQueryException(m_source, i_at, i_reason, i_message + " (path: '" + m_source + "')");
  }

  private static String describe(Object i_value)
  {
    if (i_value == null || i_value == JSONObject.NULL)
    {
      return "null";
    }
    return i_value.getClass().getSimpleName() + " (" + i_value + ")";
  }
}
