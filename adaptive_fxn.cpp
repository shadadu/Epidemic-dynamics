#include <iostream>
#include "node.h"
#include "Link.h"
#include "adaptive_fxn.h"


using namespace std;

int adaptive_fxn(int kmax, int factor, int nInfected, int Popn )
{

  double k0 = kmax*(1- factor*(nInfected/double(Popn))  );

  return k0;

}
