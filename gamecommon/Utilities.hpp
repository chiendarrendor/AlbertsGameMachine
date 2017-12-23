#ifndef UTILITIESHPP
#define UTILITIESHPP

#include <algorithm>
#include <sstream>

inline int myrand(int upper)
{
	return (int)(rand() * (float)upper / (RAND_MAX + 1.0));
}

inline int RandomBetween(int lower, int upper)
{
  return myrand(upper-lower+1) + lower;
}

inline int DieRoll()
{
	return myrand(6) + 1;
}

template<class _It> void myshuffle(_It i_Begin,_It i_End)
{
	while (i_Begin != i_End)
	{
		_It other = i_Begin;
		std::advance(other,myrand(std::distance(i_Begin,i_End)));
		std::iter_swap(other,i_Begin);
		i_Begin++;
	}
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
