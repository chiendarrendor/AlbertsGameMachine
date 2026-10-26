package org.doghouse.jsonpath;

import java.util.ArrayList;
import java.util.List;

/**
 * Recursive-descent parser for the JsonPath grammar:
 *
 *   path    := segment ('.' segment | '[' index ']' | '{' '}' | '#' op)*
 *   segment := identifier
 *   index   := (empty)        -- map: '[]'
 *            | digits          -- array position: '[3]'
 *            | '"' chars '"'   -- object key literal: '["Great Hall"]'
 *   op      := "size"
 *
 * '{}' (dict) has no variants the way '[...]' does -- it's always exactly
 * the two characters, nothing can appear between the braces.
 *
 * There is no notion of a distinguished "root" segment -- the first segment
 * is parsed and evaluated exactly like every later one. What a path's first
 * field name actually resolves to is entirely up to whatever object the
 * caller passes to JsonPath.evaluate(); this parser has no project-specific
 * vocabulary at all.
 *
 * No whitespace is permitted anywhere in a path, and quoted keys support no
 * escape sequences -- both deliberate simplifications for v1.
 */
final class PathParser
{
  private final String m_source;
  private int m_pos;

  private PathParser(String i_source)
  {
    m_source = i_source;
    m_pos = 0;
  }

  static List<Step> parse(String i_source)
  {
    PathParser parser = new PathParser(i_source);
    List<Step> steps = new ArrayList<Step>();
    steps.add(new FieldStep(parser.parseIdentifier()));
    parser.parseRest(steps);

    if (parser.m_pos != parser.m_source.length())
    {
      throw parser.syntaxError("unexpected trailing content");
    }
    return steps;
  }

  // Parses '.field' / '[...]' / '#size' repeatedly, appending to `io_steps`.
  // A '[]' (map) compiles everything remaining as a nested sub-path inside
  // a MapStep and returns immediately -- nothing can follow a map at this
  // level, only inside its own subpath.
  private void parseRest(List<Step> io_steps)
  {
    while (m_pos < m_source.length())
    {
      char c = m_source.charAt(m_pos);

      if (c == '.')
      {
        m_pos++;
        io_steps.add(new FieldStep(parseIdentifier()));
      }
      else if (c == '[')
      {
        Step step = parseBracket();
        io_steps.add(step);
        if (step instanceof MapStep)
        {
          return;
        }
      }
      else if (c == '{')
      {
        io_steps.add(parseBrace());
        return; // '{}' always consumes the rest, same as '[]'
      }
      else if (c == '#')
      {
        m_pos++;
        String op = parseIdentifier();
        if (!op.equals("size"))
        {
          throw syntaxError("unknown operator '#" + op + "'");
        }
        io_steps.add(new SizeStep());
        if (m_pos != m_source.length())
        {
          throw syntaxError("'#size' must be the last segment of a path");
        }
        return;
      }
      else
      {
        throw syntaxError("unexpected character '" + c + "'");
      }
    }
  }

  // Precondition: m_source.charAt(m_pos) == '['.
  private Step parseBracket()
  {
    m_pos++; // consume '['

    if (m_pos < m_source.length() && m_source.charAt(m_pos) == ']')
    {
      m_pos++;
      List<Step> subpath = new ArrayList<Step>();
      parseRest(subpath);
      return new MapStep(subpath);
    }

    if (m_pos < m_source.length() && m_source.charAt(m_pos) == '"')
    {
      m_pos++;
      int start = m_pos;
      while (m_pos < m_source.length() && m_source.charAt(m_pos) != '"')
      {
        m_pos++;
      }
      if (m_pos >= m_source.length())
      {
        throw syntaxError("unterminated string literal");
      }
      String key = m_source.substring(start, m_pos);
      m_pos++; // consume closing '"'
      expect(']');
      return new FieldStep(key);
    }

    int start = m_pos;
    while (m_pos < m_source.length() && Character.isDigit(m_source.charAt(m_pos)))
    {
      m_pos++;
    }
    if (m_pos == start)
    {
      throw syntaxError("expected an array index, a quoted object key, or ']' for a map");
    }
    int index = Integer.parseInt(m_source.substring(start, m_pos));
    expect(']');
    return new IndexStep(index);
  }

  // Precondition: m_source.charAt(m_pos) == '{'.
  private Step parseBrace()
  {
    m_pos++; // consume '{'
    expect('}');
    List<Step> subpath = new ArrayList<Step>();
    parseRest(subpath);
    return new DictStep(subpath);
  }

  private String parseIdentifier()
  {
    int start = m_pos;
    while (m_pos < m_source.length() &&
           (Character.isLetterOrDigit(m_source.charAt(m_pos)) || m_source.charAt(m_pos) == '_'))
    {
      m_pos++;
    }
    if (m_pos == start)
    {
      throw syntaxError("expected an identifier");
    }
    return m_source.substring(start, m_pos);
  }

  private void expect(char i_c)
  {
    if (m_pos >= m_source.length() || m_source.charAt(m_pos) != i_c)
    {
      throw syntaxError("expected '" + i_c + "'");
    }
    m_pos++;
  }

  private JsonPathSyntaxException syntaxError(String i_message)
  {
    return new JsonPathSyntaxException(m_source, m_pos, i_message);
  }
}
