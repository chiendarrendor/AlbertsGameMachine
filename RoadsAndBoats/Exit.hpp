//-- My documentation about C++ is written like this
//-- two slashes indicate a comment... as does any text between /*  and */


//-- anything with a '#' as the first character in a line is an instruction
//-- to the 'preprocessor' which does various kinds of text replacement
//-- before the compiler actually starts doing work.

//-- #include "filename"
//-- #include <filename>
//--    --copy the named file (send to the compiler) the given filename
//--      at this location in the file. (this is to build up header dependencies
//--      needed to compile external references in a given piece of code)
//--      the "" form looks in the user-defined directories for the file first, then 
//--      the system include directory, while the <> form looks in the system
//--      include directory first.
//-- #ifndef EXITHPP / #endif (down at the bottom of the file)
//--   means 'output (send to the compiler) everything between the ifndef and the endif
//--   as long as the preprocessor variable EXITHPP is _not_ defined
//-- #define means to define the value of a preprocessor variable
//-- the purpose of this is that if a given set of files that all
//-- are dependent on each other (i.e. have  #includes) in such a way
//-- that the same file ends up included twice, that each class is still only defined once
//-- (it is an error in C++ for the compiler to see two different class definitions with
//-- the same name)
//-- this is a very standardized coding pattern that you will see in many C and C++
//-- programs of any significant scale
#ifndef EXITHPP
#define EXITHPP


// specifies an exit out of a hex
// edges are:
// 
//   1 ^ 2
//    / \
// 6 |   | 3
//    \ /
//   5 V 4
//
// 'SUNWISE' means clockwise from the center of the edge
// 'WIDDERSHINS' means counterclockwise

//-- class definition, much like java, except no 'public' before the word class...
//-- all classes are public.
class Exit
{
  //-- as mentioned, the publicity of methods and attributes are determined
  //-- in C++ in public/protected/private blocks, instead of individually as in java.
  //-- otherwise, behaviour is identical.
public:
  enum Rotation { WIDDERSHINS, CENTER , SUNWISE };

  //--default constructor
  Exit();
  //--copy constructor -- required for passing an object of this class in and out via
  //--function calls, and a few other places.  like the assignment operator (see below)
  //--default behaviour is a bitwise copy.
  Exit(const Exit &i_right);
  //--non-default assignment operator
  //-- if you write a=b; in C++, where a and b are instances of classes,
  //-- and do not define an assignment operator, it will do a bit-wise copy of the
  //-- memory of right hand side to the memory of the right hand side.  For cases, like
  //-- this one, where the only data in the class is PODS (ints in this case), a bitwise
  //-- copy will have correct behaviour, but given that even very simple non PODS data types
  //-- (like std::string, for instance) will behave incorrectly under bitwise copy, it is considered
  //-- good form to either write your own copy constructor and assignment operator, or to specifically
  //-- define them private and don't implement them, to make sure that any accidental use of that
  //-- functionality is met with a compile error.
  Exit &operator=(const Exit &i_right);
  //-- 'normal' constructor.  note the second argument has a 'default argument'
  //-- i.e.  you may say Exit myExit(3), and this constructor will still be called, but CENTER will automatically
  //-- be used as the missing second argument.
  Exit(int i_edge,Rotation i_rotation = CENTER);

  //-- const, as I described earlier, is a mechanism that creates a contract of non-modification.
  //-- marking a method const means that the compiler will refuse to compile any behaviour into this
  //-- method that modifies the attributes of this class, and will allow only const-marked methods
  //-- to be called when presented with a const object.
  int GetEdge() const;
  Rotation GetRotation() const;
private:
  int m_edge;
  int m_rotation;
};

//-- this method is required in order to use objects of this class as keys of std::map<>
bool operator<(const Exit &i_left,const Exit &i_right);



#endif
