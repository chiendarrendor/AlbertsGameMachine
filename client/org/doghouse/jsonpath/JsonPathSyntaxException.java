package org.doghouse.jsonpath;

/**
 * A path string failed to parse -- bad syntax in the expression itself
 * (unbalanced brackets, an empty identifier, '#size' with trailing content,
 * ...). Always a compile-time ({@link JsonPath#compile}) failure, never
 * thrown by {@link JsonPath#evaluate}. Unlike evaluation failures, there is
 * no default-value escape hatch for this -- a malformed path string is an
 * authoring bug, not a data-shape mismatch, and should never be silently
 * swallowed.
 */
public class JsonPathSyntaxException extends RuntimeException
{
  private final String m_path;
  private final int m_position;

  public JsonPathSyntaxException(String i_path, int i_position, String i_message)
  {
    super(i_message + " at position " + i_position + " in '" + i_path + "'");
    m_path = i_path;
    m_position = i_position;
  }

  public String getPath()
  {
    return m_path;
  }

  public int getPosition()
  {
    return m_position;
  }
}
