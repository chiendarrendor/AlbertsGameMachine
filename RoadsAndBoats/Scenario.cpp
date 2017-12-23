#include "Scenario.hpp"
#include <map>
#include <iostream>
#include <fstream>
#include <exception>
#include <boost/lexical_cast.hpp>
#include "StringUtilities.hpp"

namespace
{
  bool isStart(const std::string &i_line, std::string &i_name)
  {
    int i;
    for (i = 0 ; i < i_line.size() ; ++i)
    {
      if (i_line[i] == ':')
      {
        i_name = std::string(i_line.c_str(),i);
        return true;
      }
    }
    return false;
  }

  bool splitFile(const std::string &i_fname,
                 std::map<std::string,std::string> &i_map)
  {
    std::ifstream ifs(i_fname.c_str());
    if (!ifs) return false;
    
    std::string buf;
    bool inBlock = false;
    std::string blockname;
    std::string body;
    std::string tname;

    while(std::getline(ifs,buf))
    {
      if (buf.size() == 0) continue;
      if (buf[0] == '#') continue;
      if (inBlock && buf == ".end.")
      {
        inBlock = false;
        i_map[blockname] += body;
        body = "";
        continue;
      }
      if (!inBlock && isStart(buf,tname))
      {
        blockname = tname;
        inBlock = true;
        continue;
      }
      if (inBlock)
      {
        body += buf;
        body += "\n";
      }
    }
    return true;
  }
}

Scenario::Scenario(const std::string &i_fname)
{
  m_isValid = true;
  std::map<std::string,std::string> splits;
  if (!splitFile(i_fname,splits))
  {
    m_why = "Unsplittable file";
    m_isValid = false;
    return;
  }


  if (splits.find("Name") != splits.end())
  {
    m_name = std::string(splits["Name"],0,splits["Name"].size()-1);
  }
  else
  {
    m_why = "no Name block";
    m_isValid = false;
    return;
  }


  if (splits.find("Comment") != splits.end())
  {
    m_comment = splits["Comment"];
  }

  if (splits.find("NumPlayers") != splits.end())
  {
    m_numplayers = atoi(splits["NumPlayers"].c_str());
  }
  else
  {
    m_why = "no NumPlayers block";
    m_isValid = false;
    return;
  }

  if (splits.find("Board") == splits.end())
  {
    m_why = "no board block";
    m_isValid = false;
    return;
  }

  std::vector<std::string> rows;
  Tokenize(rows,splits["Board"],"\n");

  m_height = rows.size();
  int i,j,k;
  m_width = -1;
  
  int numStarting = 0;

  for (i = 0 ; i < m_height ; ++i)
  {
    std::vector<std::string> cols;
    Tokenize(cols,rows[i]," ");
    if (m_width == -1)
    {
      m_width = cols.size();
      
    }
    else if (m_width != cols.size())
    {
      m_why = "inconsistent widths";
      m_isValid = false;
      return;
    }

    for( j = 0 ; j < cols.size() ; ++j)
    {
      TileType tt;

      switch(cols[j][0])
      {
      case '.': tt = EMPTY; break;
      case 'f': tt = WOODS; break;
      case 'p': tt = PASTURE; break;
      case 's': tt = ROCK; break;
      case 'm': tt = MOUNTAINS; break;
      case 'd': tt = DESERT; break;
      case 'w': tt = SEA; break;
      default:
        m_why = "unknown tile type";
        m_isValid = false;
        return;
      }

      ScenarioHex sh(tt);

      // any of the above is fine with no suffixes.
      if (cols[j].size() == 1)
      {
        m_hexes.push_back(sh);
        continue;
      }        

      // if we get here, we know we have at least one character
      // beyond the land type

      // empty and sea spaces can have neither
      // rivers nor starting spaces.
      if (tt == EMPTY || tt == SEA)
      {
        m_why = "river/starting space on empty/sea";
        m_isValid = false;
        return;
      }

      // the suffix can be '*', but only if
      // it is alone.
      if (cols[j][1] == '*')
      {
        if (cols[j].size() == 2)
        {
          sh.SetStartingSpace(1);
          m_hexes.push_back(sh);
          numStarting++;
          continue;
        }
        else
        {
          m_why = "first char of multi-char suffix is '*'";
          m_isValid = false;
          return;
        }
      }

      // if we are here, the following is true:
      // * we have at least one character of suffix
      // * our land is neither EMPTY nor SEA
      // * the first character is not '*'
      // the rest of the suffix must be of the form
      // ([1-6]\*?)+

      int prevnum = -1;
      for (k = 1 ; k < cols[j].size() ; ++k)
      {
        int cn = cols[j][k] - '0';
        if (cn >= 1 && cn <= 6)
        {
          sh.SetRiver(cn);
          prevnum = cn;
        }
        else if (cols[j][k] == '*')
        {
          if (prevnum == -1)
          {
            m_why = "multiple '*' in suffix";
            m_isValid = false;
            return;
          }
          numStarting++;
          sh.SetStartingSpace(prevnum);
          prevnum = -1;
        }
        else
        {
          m_why = "unknown char in suffix";
          m_isValid = false;
          return;
        }
      }
      m_hexes.push_back(sh);
    }
  }
  
  // once we get here, the whole board is parsed.
  // last check: the board should either have
  // zero starting spaces (which implies that
  // all legal spots are starting spaces)
  // or at least as many as the number of
  // players.

  if (numStarting > 0)
  {
    if (numStarting < GetNumPlayers())
    {
      m_why = "non-zero starting spaces < num players";
      m_isValid = false;
    }
    return;
  }

  // if we get here, numStarting is zero,
  // so we need to set a starting space for every legal space.
  for (i = 0 ; i < m_hexes.size() ; ++i)
  {
    if (m_hexes[i].GetTileType() == EMPTY ||
        m_hexes[i].GetTileType() == SEA ||
        m_hexes[i].GetTileType() == DESERT)
    {
      continue;
    }

    if (m_hexes[i].GetRivers().size() < 2)
    {
      m_hexes[i].SetStartingSpace(1);
    }
    else
    {
      std::set<int>::const_iterator cit;
      for(cit = m_hexes[i].GetRivers().begin();
          cit != m_hexes[i].GetRivers().end();
          ++cit)
      {
        m_hexes[i].SetStartingSpace(*cit);
      }
    }
  }

  // last check... all spaces with rivers in them must
  // connect to either:
  // a. off the board
  // b. into a sea space
  // c. into an empty space
  // d. into a countervailing river in the space in that direction
  for (i = 0 ; i < GetWidth() ; ++i)
  {
    for (j = 0 ; j < GetHeight() ; ++j)
    {
      const ScenarioHex &sh = GetScenarioHex(i,j);
      std::set<int>::const_iterator rit;
      for (rit = sh.GetRivers().begin() ; rit != sh.GetRivers().end() ; ++rit)
      {
        int dir = *rit;
        // find adjacent space
        int nx = HexDirX(i,j,dir);
        int ny = HexDirY(i,j,dir);
        int cdir = BackDir(dir);
        
        // space is off board
        if (nx < 0 || ny < 0 || nx >= GetWidth() || ny >= GetHeight()) continue;

        const ScenarioHex &nsh = GetScenarioHex(nx,ny);
        // space is empty(also off board) or sea.
        if (nsh.GetTileType() == EMPTY || nsh.GetTileType() == SEA) continue;

        // check to see if there is a counterflowing river in the other space
        if (nsh.GetRivers().find(cdir) != nsh.GetRivers().end()) continue;
        
        // if we get here, there's a problem.
        m_why = "river link failure.";
        m_isValid = false;
        return;
      }
    }
  }
}

const std::string &Scenario::GetName() const
{
  return m_name;
}

const std::string &Scenario::GetComment() const
{
  return m_comment;
}

int Scenario::GetNumPlayers() const
{
  return m_numplayers;
}

int Scenario::GetWidth() const
{
  return m_width;
}

int Scenario::GetHeight() const
{
  return m_height;
}

bool Scenario::IsValid() const
{
  return m_isValid;
}


int Scenario::GetHexIndex(int i_x,int i_y) const
{
  return i_y * GetWidth() + i_x;
}

const ScenarioHex &Scenario::GetScenarioHex(int i_x, int i_y) const
{
  return m_hexes.at(GetHexIndex(i_x,i_y));
}

const std::string &Scenario::Why() const
{
  return m_why;
}
