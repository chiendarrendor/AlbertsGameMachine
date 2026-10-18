package org.doghouse.jsonpath;

/**
 * A compiled path failed to resolve against real data: it needed to
 * navigate through a JSON null, a key/index was genuinely absent, or an
 * operator was applied to a value of the wrong kind (e.g. '#size' on a
 * scalar). Reaching a JSON null as a path's *final* value is never this
 * exception -- that is a normal, successful result (see JsonPath.evaluate).
 *
 * {@link JsonPath#evaluate(Object, Object)} catches this and returns the
 * supplied default instead of throwing; {@link JsonPath#evaluate(Object)}
 * lets it propagate.
 */
public class JsonQueryException extends RuntimeException
{
  public enum Reason
  {
    NULL_ENCOUNTERED,
    KEY_ABSENT,
    INDEX_OUT_OF_BOUNDS,
    TYPE_MISMATCH
  }

  private final String m_path;
  private final int m_failedAt;
  private final Reason m_reason;

  public JsonQueryException(String i_path, int i_failedAt, Reason i_reason, String i_message)
  {
    super(i_message);
    m_path = i_path;
    m_failedAt = i_failedAt;
    m_reason = i_reason;
  }

  public String getPath()
  {
    return m_path;
  }

  /** Index, within the compiled step list, of the step that failed. */
  public int getFailedAt()
  {
    return m_failedAt;
  }

  public Reason getReason()
  {
    return m_reason;
  }
}
