#include <string>

#ifdef msys
#include <windows.h>
#define LIBPTR HINSTANCE
#elif defined(FreeBSD) || defined(linux)
#include <dlfcn.h>
#define LIBPTR void *
#endif


class GenericLibraryLoader
{
public:
  GenericLibraryLoader(const std::string &i_prefix);
  virtual ~GenericLibraryLoader();
  
  bool IsOk() const;
  void *GetFunctionAddress(const std::string &i_procname) const;
  // if IsOk is false, or if GetFunctionAddress returns null, this may have something interesting to say
  std::string GetErrorString();
private:
  LIBPTR m_library;
  std::string m_libname;
};


