#include <string>

class ServerGameInfo;
class OutputPort;
class ActionParser;

class Game
{
public:
  Game(const ServerGameInfo &i_rServerGameInfo,OutputPort &i_rOutputPort);
  virtual ~Game() {}
  const ServerGameInfo &GetServerGameInfo() const;

  // implemented interface -- cognizant of where to load and store
  // the files
  bool Load(const std::string &i_FileName);
  bool Save(const std::string &i_FileName);

  virtual std::string GetStatusString() const = 0;
  virtual std::string GetName() const = 0;
  virtual bool IsDone() const = 0;
  virtual void SendFullState(const std::string &i_Name) const = 0;
  virtual void HandleAction(const std::string &i_Name,const ActionParser &i_ap) = 0;

  // interface to implement -- should read and write to the exact file names given.
  virtual bool LoadFile(const std::string &i_FileName) = 0;
  virtual bool SaveFile(const std::string &i_FileName) const = 0;

protected:
  OutputPort &GetOutputPort() const;
private:
  OutputPort &m_rOutputPort;
  const ServerGameInfo &m_rServerGameInfo;
};
