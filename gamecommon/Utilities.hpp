#ifndef UTILITIESHPP
#define UTILITIESHPP

#include <algorithm>
#include <sstream>
#include "Random.hpp"

inline int myrand(int upper)
{
	return Random::Instance().MyRand(upper);
}

inline int RandomBetween(int lower, int upper)
{
  return Random::Instance().Between(lower,upper);
}

inline int DieRoll()
{
	return Random::Instance().DieRoll();
}

template<class _It> void myshuffle(_It i_Begin,_It i_End)
{
	Random::Instance().Shuffle(i_Begin,i_End);
}

template<class _Ctr> std::string join(_Ctr i_ctr)
{
	std::ostringstream oss;

	typename _Ctr::const_iterator it;
	for (it = i_ctr.begin() ; it != i_ctr.end() ; it++)
	{
		if (it != i_ctr.begin())
		{
			oss << ",";
		}
		oss << *it;
	}
	return oss.str();
}



#endif
