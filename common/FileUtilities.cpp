#include "FileUtilities.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

bool FileExists(const std::string &i_Name)
{
  struct stat buf;
  int result;

   /* Get data associated with "stat.c": */
   result = stat(i_Name.c_str(), &buf);

   if (result == 0)
   {
     return true;
   }

   if (result == -1 && errno == ENOENT)
   {
     return false;
   }
   
   throw FileUtilitiesException("Stat Failed in FileExists");
}

bool MakeDirectory(const std::string &i_Name)
{
#if msys
  int result = mkdir(i_Name.c_str());
#else
  int result = mkdir(i_Name.c_str(),0755);
#endif

  if (result == 0)
  {
    return true;
  }

  return false;
}

bool GetDirectoryContents(const std::string &i_Name,std::vector<std::string> &i_Entries)
{
  DIR *pDir = opendir(i_Name.c_str());

  if (!pDir)
  {
    return false;
  }

  struct dirent *pDirEnt;

  while((pDirEnt = readdir(pDir)))
  {
    std::string dirname(pDirEnt->d_name);
    if (dirname == "." || dirname == "..") continue;

    i_Entries.push_back(dirname);
  }

  return true;
};


