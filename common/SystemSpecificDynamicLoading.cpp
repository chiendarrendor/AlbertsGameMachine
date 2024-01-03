#include <boost/lexical_cast.hpp>
#include "SystemSpecificDynamicLoading.hpp"
#ifdef msys


GenericLibraryLoader::GenericLibraryLoader(const std::string &i_prefix) :
  m_library(NULL),
  m_libname(i_prefix + ".dll")
{
  m_library = LoadLibrary(m_libname.c_str());
}

GenericLibraryLoader::~GenericLibraryLoader()
{
  if (m_library) FreeLibrary(m_library);
}

void *GenericLibraryLoader::GetFunctionAddress(const std::string &i_procname) const
{
  return (void *)GetProcAddress(m_library,i_procname.c_str());
}

std::string GenericLibraryLoader::GetErrorString()
{
  return m_libname + ": " + boost::lexical_cast<std::string>(GetLastError());
}

#elif defined(FreeBSD) || defined(linux)

GenericLibraryLoader::GenericLibraryLoader(const std::string &i_prefix) :
  m_library(NULL)
{
  std::string fullname = i_prefix + ".so";
  m_library = dlopen(fullname.c_str(),RTLD_NOW);
}

GenericLibraryLoader::~GenericLibraryLoader()
{
  if (m_library) dlclose(m_library);
}

void *GenericLibraryLoader::GetFunctionAddress(const std::string &i_procname) const
{
  return dlsym(m_library,i_procname.c_str());
}


#include <errno.h>

std::string GenericLibraryLoader::GetErrorString()
{
  return m_libname + ": " + boost::lexical_cast<std::string>(errno);
}

#endif



bool GenericLibraryLoader::IsOk() const
{
  return (m_library != NULL);
}
