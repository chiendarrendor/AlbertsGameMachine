#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/map.hpp>

// text forms
//#define SERIALIZE(x) i_ar & (x)
//#define SERIALIZE_PARENT(PARTYPE) i_ar & boost::serialization::base_object<PARTYPE>(*this)

// xml forms
#define SERIALIZE(x) i_ar & BOOST_SERIALIZATION_NVP(x)
#define SERIALIZE_PARENT(PARTYPE) i_ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(PARTYPE)


#define SERIALIZE_FUNC \
friend class boost::serialization::access; \
template<class Archive> void serialize(Archive &i_ar,const unsigned int version)

#define SERIALIZE_FRIENDSHIP friend class boost::serialization::access

#define SERIALIZE_LOAD_FUNC  \
template<class Archive> void load(Archive &i_ar,const unsigned int version)

#define SERIALIZE_SAVE_FUNC  \
template<class Archive> void save(Archive &i_ar,const unsigned int version) const



typedef boost::archive::xml_oarchive SaveArchive;
typedef boost::archive::xml_iarchive LoadArchive;

// to serialize:
// within classes:
// private:
//   SERIALIZE_FUNC
//   {
//     SERIALIZE(var1);
//     SERIALIZE(var2);
//   }
// if class is a subclass:
// private:
//   SERIALIZE_FUNC
//   {
//     SERIALIZE_PARENT(<parent class name>);
//     SERIALIZE(var3);
//     SERIALISE(var4);
//
// 
// to make serialization versions:
// if (version > x)
//     SERIALIZE(new item)
//
// BOOST_CLASS_VERSION(<class name>,version)
