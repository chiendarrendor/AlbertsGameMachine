#ifndef RANDOMFACTORYHPP
#define RANDOMFACTORYHPP

class RandomFactory
{
public:
  virtual ~RandomFactory() {}

  // returns a value in [0, i_upper)
  virtual int Next(int i_upper) = 0;
};

#endif
