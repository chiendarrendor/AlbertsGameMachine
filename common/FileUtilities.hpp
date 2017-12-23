
#include <string>
#include <exception>
#include <vector>


#ifdef msys
#define DIR_SEP '\\'
#else
#define DIR_SEP '/'
#endif

class FileUtilitiesException : public std::exception
{
public:
  FileUtilitiesException(const char *i_pName) : m_pName(i_pName) {}
  virtual const char* what() const throw() { return m_pName; }
private:
  const char *m_pName;
};

bool FileExists(const std::string &i_Name);
bool MakeDirectory(const std::string &i_Name);
bool GetDirectoryContents(const std::string &i_Name,std::vector<std::string> &i_Entries);

