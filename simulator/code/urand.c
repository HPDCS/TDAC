#include "stdafx.h"

#include <math.h>

double seed=0;
double seedn=0;
double mask=(double)0x7fffffffL;
double iseed=470211272.0;


double urand() {
	  if (seed==0) {
	    seed=iseed;
	  } else {
	    seed*=16807.0;
	    //seed=fmod(seed, mask);
	  }
  return(seed/mask);
}

void surand(double s) {
  seed=65536.0*s;
}
