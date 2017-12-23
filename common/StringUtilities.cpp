
#include "StringUtilities.hpp"
#include <ctype.h>

// private function for Tokenize to make it easier to read...
bool IsCharASeparator(char i_Char,const std::string &i_Seps)
{
	return(i_Seps.find_first_of(i_Char) != std::string::npos);
}


// given an input string, and a string containing separators, return
// a vector of the non-empty strings occuring between start, each 
// separator character found, and end. If AllowEmpties is set
// to true, then it will return a vector of all strings, empty or
// not, between start, each separator, and end.

void Tokenize(std::vector<std::string> &o_ExtVector,
							const std::string &i_ToToken,
							const std::string &i_Seps,
							bool i_AllowEmpties)
{

	o_ExtVector.clear();

	// pstart is is the index of the first character
	// in the next string being found.
	int pstart = 0;

	// eating is true, if we are in the process of
	// "eating" adjacent separator characters
	// if we do not allow empties, we want to "eat"
	// all separators up to the first non-separator,
	// so we set this to true.
	// if we are allowing empties, we want this false.
	bool eating = !i_AllowEmpties;

	// i is the index of the character currently being 
	// evaluated.
	size_t i;

	// for every character
	for (i = 0 ; i < i_ToToken.size() ; i++)
	{
		char curChar = i_ToToken[i];

		// if we are eating, and we see a separator,
		// continue eating (pushing pstart forward)
		if (eating && IsCharASeparator(curChar,i_Seps))
		{
			pstart++;
		}
		// if we are eating, and we see another character,
		// we're done eating, and now we need to look for
		// the other end of the string.
		else if (eating)
		{
			eating = false;
		}
		// if we're here, we're not eating, and we found
		// a separator.  
		// example, pstart is at position 3,
		// and i is position 6.
		// pstart is the first non-separator character in
		// this string, and i is a separator.
		// so, we want the substring starting at 3, and 
		// including characters 3,4, and 5, hence
		// length is i - pstart
		// also, if, for example, pstart is 0 and
		// the first character is a separator (in allowempties)
		// then we are going to be taking substr(0,0-0)
		// in either case, we want pstart to point to the
		// character we're about to look at (as opposed to leaving
		// it here)

		else if (IsCharASeparator(curChar,i_Seps))
		{
			std::string el = i_ToToken.substr(pstart,i-pstart);
			o_ExtVector.push_back(el);
			eating = !i_AllowEmpties;
			pstart = i+1;
		}
	}
	// if we've reached the end, and we're not eating separators,
	// we want to generate one more string.
	if (!eating)
	{
		std::string el = i_ToToken.substr(pstart,i-pstart);
		o_ExtVector.push_back(el);
	}
}

std::string PadTo(const std::string &i_str,int i_Width)
{
  std::string result = i_str;

  int npad,i;

  npad = i_Width - result.length();

  for (i = 0 ; i < npad ; i++)
  {
    result += " ";
  }

  return result;
}

bool IsCaseInsensitveMatch( const std::string & i_str1, const std::string & i_str2 )
{
	if( i_str1.size() != i_str2.size() ) return false;

	std::string::const_iterator s1Itr = i_str1.begin();
	std::string::const_iterator s2Itr = i_str2.begin();

	while( s1Itr != i_str1.end() )
	{
		if (toupper(*s1Itr++) != toupper(*s2Itr++)) return false;
	}

	return true;
}

void RemoveWhitespace( std::string & io_str )
{
	size_t idx = 0;

	// remove leading whitespace
	for( ; idx < io_str.size(); idx++ )
	{
		if( !isspace( io_str[idx] ) )
		{
			break;
		}
	}
	if( idx > 0 )
	{
		io_str = io_str.substr( idx );
	}

	// remove ending whitespace
	idx = io_str.size();
	for( ; 0 < idx ; idx-- )
	{
		if( !isspace( io_str[idx-1] ) )
		{
			break;
		}
	}
	if( idx < io_str.size() )
	{
		io_str = io_str.substr( 0, idx );
	}
}

std::string UnComma(const std::string i_Source)
{
	std::string result;

	size_t i;
	for (i = 0 ; i < i_Source.size() ; ++i)
	{
		switch(i_Source[i])
		{
		case ',':
			result += '%';
			result += 'C';
			break;
		case '%':
			result += '%';
			result += '%';
			break;
		default:
			result += i_Source[i];
		}
	}
	return result;
}

std::string ReComma(const std::string i_Source)
{
	std::string result;

	size_t i;
	for (i = 0 ; i < i_Source.size() ; ++i)
	{
		char c = i_Source[i];
		if (c != '%')
		{
			result += c;
			continue;
		}
		if (i == i_Source.size() - 1)
		{
			result += c;
			continue;
		}
		char cplus = i_Source[i+1];
		if (cplus == 'C')
		{
			result += ',';
			i++;
			continue;
		}
		if (cplus == '%')
		{
			result += '%';
			i++;
			continue;
		}
		result += c;
	}
	return result;
}




