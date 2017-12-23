#include <set>
#include <map>
#include <string>

class GameBox;

class GameCloset
{
public:
  GameCloset(const std::string &i_rDataFile,const std::string &i_rDataDir);

  const std::set<std::string> &GetGameBoxNames() const;
  const GameBox *GetGameBoxByName(const std::string &i_Name) const;

private:
  std::set<std::string> m_GameBoxNames;
  std::map<std::string,GameBox *> m_GameBoxes;
};

