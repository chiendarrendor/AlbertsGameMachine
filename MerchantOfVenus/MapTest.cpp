#include "MapReader.hpp"
#include <exception>
#include <iostream>
#include <stdio.h>

int main(int argc,char **argv)
{
  try
  {
    MapReader mr(argv[1]);
    mr.Visualize();
  }
  catch(std::exception&  e)
  {
    std::cout << "Exception Caught: " << e.what() << std::endl;
  }
}

  
  
