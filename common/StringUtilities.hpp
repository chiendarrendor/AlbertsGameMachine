//
// FILE NAME:		$RCSfile: StringUtilities.hpp,v $	
//
// REVISION:		$Revision: 1.1 $
// 
// COPYRIGHT:		(c) 2001 Advertising.com All Rights Reserved.
// 
// LAST UPDATED:	$Date: 2012/06/16 02:07:42 $
// UPDATED BY:		$Author: albert $
// 
// DESCRIPTION:	This file has some string utilities we find usefule for our efforts!
//

#ifndef __StringUtilities__
#define __StringUtilities__
#include <string>
#include <stdlib.h>
#ifdef FreeBSD
#include <stdio.h>
#elif defined(msys)
#endif

#include <vector>
#include <sstream>

//
// method declarations
//

std::string& operator+=( std::string& i_String, const int& i_IntValue );
std::string& operator+=( std::string& i_String, const unsigned long& i_ULValue );
std::string& operator+=( std::string& i_String, const double& i_DoubleValue );
std::string operator+(const std::string &i_String,const int &i_IntValue);


//
// Implementations
//

inline std::string 
operator+(const std::string &i_String,const int &i_IntValue)
{
  std::ostringstream oss;
  oss << i_String << i_IntValue;
  std::string result = oss.str();
  return result;
}

inline std::string&
operator+=( std::string& i_String, const int& i_IntValue )
{
  std::ostringstream oss;
  oss << i_IntValue;
  i_String += oss.str();
  return i_String;
}
inline std::string&
operator+=( std::string& i_String, const unsigned long& i_ULValue )
{
  std::ostringstream oss;
  oss << i_ULValue;
  i_String += oss.str();
  return i_String;
}


inline std::string&
operator+=( std::string& i_String, const double& i_DoubleValue )
{
  std::ostringstream oss;
  oss << i_DoubleValue;
  i_String += oss.str();
  return i_String;
}

// given an input string, and a string containing separators, return
// a vector of the non-empty strings occuring between start, each 
// separator character found, and end. If AllowEmpties is set
// to true, then it will return a vector of all strings, empty or
// not, between start, each separator, and end.
void Tokenize(std::vector<std::string> &o_ExtVector,
							const std::string &i_ToToken,
							const std::string &i_Seps,
							bool i_AllowEmpties = false);


// returns a string that is i_str, followed by as many
// spaces as to make it i_width in width.  If i_str
// is already longer, returns it.
std::string PadTo(const std::string &i_str,int i_width);

// Do no-case comparison of two string.
//  returns:
//  true, if upper case of each corresponding element of i_str1 and i_str2
//        is the same, and the two string has same length.
//  false, otherwise. 
bool IsCaseInsensitveMatch( const std::string & i_str1, const std::string & i_str2 );

// Remove leading and trailing whitespace from a string.
void RemoveWhitespace( std::string & io_str );

std::string UnComma(const std::string i_Input);
std::string ReComma(const std::string i_Input);

#endif
