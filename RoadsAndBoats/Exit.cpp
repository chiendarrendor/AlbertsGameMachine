//--each .cpp file must include it's corresponding .hpp file, in order to have the 
//--class definition of the methods that are implemented in this .cpp file.
#include "Exit.hpp"

//-- note the 'initialization list' in the constructors.  The simplest explanation
//-- of this is that it is the correct C++ way to assign the values of attributes
//-- in the constructor...the truth is somewhat more involved, but tends to only
//-- come up rarely.  you _can_ assign values to attributes in the body
//-- of the constructor too, like you do in java, but this is, in most cases,
//-- considered poor style.
Exit::Exit() :
  m_edge(0),
  m_rotation(Exit::CENTER);
{
}

Exit::Exit(const Exit &i_right) :
  m_edge(i_right.m_edge),
  m_rotation(i_right.m_rotation)
{
}

Exit::Exit(int i_edge,Rotation i_rotation) :
  m_edge(i_edge),
  m_rotation(i_rotation)
{
}

//-- assigment operator... note the test that ensure that this (which is a pointer)
//-- is different from the address of i_right (also a pointer).  depending on
//-- the sophistication of your class, it occasionally causes problems in the case
//-- where you assign to yourself, so it is considered good form to always put this 
//-- check in.
Exit &Exit::operator=(const Exit &i_right)
{
  if (this != &i_right)
  {
    m_edge = i_right.GetEdge();
    m_rotation = i_right.GetRotation();
  }
  return *this;
}



int Exit::GetEdge() const
{
  return m_edge;
}

Exit::Rotation Exit::GetRotation() const
{
  return m_rotation;
}
