#ifndef MERCHANTOFVENUSHPP
#define MERCHANTOFVENUSHPP

#include <sstream>
#include <stdexcept>


#define MAXNUMPLAYERS 6

#define THROW(M_EXCEPT,M_STREAM) \
{ \
 std::ostringstream oss; \
 oss << M_STREAM; \
 throw M_EXCEPT(oss.str().c_str());             \
}

#endif

