
#ifndef SERVERGAMEINFO
#define SERVERGAMEINFO

#include <string>

class ServerGameInfo
{
public:
  ServerGameInfo(const std::string &i_Name,
                 const std::string &i_DataDir,
                 const std::string &i_XMLLoc,
                 const std::string &i_XMLFile);

  virtual ~ServerGameInfo();

  const std::string &GetName() const;
  const std::string &GetSaveDir() const;
  const std::string &GetXMLLoc() const;
  const std::string &GetXMLFile() const;

  bool IsDirectoryValid() const;

private:
  std::string m_Name;
  std::string m_SaveDir;
  std::string m_XMLLoc;
  std::string m_XMLFile;
  bool m_IsValid;
};

#endif
