#ifndef SCENARIOHPP
#define SCENARIOHPP

#include "ScenarioHex.hpp"
#include <vector>
#include <string>

class Scenario
{
public:
  Scenario(const std::string &i_fname);
  const std::string &GetName() const;
  const std::string &GetComment() const;
  int GetNumPlayers() const;
  int GetWidth() const;
  int GetHeight() const;
  bool IsValid() const;
  const std::string &Why() const;

  const ScenarioHex &GetScenarioHex(int i_x,int i_y) const;

private:
  std::string m_name;
  std::string m_comment;
  int m_numplayers;
  int m_width;
  int m_height;
  bool m_isValid;

  int GetHexIndex(int i_x,int i_y) const;

  std::vector<ScenarioHex> m_hexes;
  std::string m_why;
};

#endif
