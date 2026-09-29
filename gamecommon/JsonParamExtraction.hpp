#ifndef JSONPARAMEXTRACTIONHPP
#define JSONPARAMEXTRACTIONHPP

#include <boost/json.hpp>
#include <string>

// Type-safe extraction of a single named field from an Action's params
// object, used by transitioncompiler-generated *_ExecuteAction methods in
// place of the old positional boost::lexical_cast<T>(i_ap[N]) pattern.
// Throws (boost::json's own exceptions, e.g. on a missing key or a value of
// the wrong kind) on a missing or wrong-shaped field -- generated code wraps
// the whole per-argument extraction block in a try/catch and reports a
// graceful UnicastERROR on any such mismatch, matching pre-JSON behavior.
template <class T>
T ExtractJsonParam(const boost::json::object &i_Params,const std::string &i_Name);

template <>
inline bool ExtractJsonParam<bool>(const boost::json::object &i_Params,const std::string &i_Name)
{
	return i_Params.at(i_Name).as_bool();
}

template <>
inline int ExtractJsonParam<int>(const boost::json::object &i_Params,const std::string &i_Name)
{
	return static_cast<int>(i_Params.at(i_Name).as_int64());
}

template <>
inline size_t ExtractJsonParam<size_t>(const boost::json::object &i_Params,const std::string &i_Name)
{
	return static_cast<size_t>(i_Params.at(i_Name).as_int64());
}

template <>
inline std::string ExtractJsonParam<std::string>(const boost::json::object &i_Params,const std::string &i_Name)
{
	return std::string(i_Params.at(i_Name).as_string().c_str());
}

#endif
