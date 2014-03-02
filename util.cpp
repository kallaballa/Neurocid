/*
 * util.cpp
 *
 *  Created on: Mar 2, 2014
 *      Author: elchaschab
 */

#include "util.hpp"
#include <cstdlib>

namespace tankwar {

int iRand(int x,int y) {
	return rand()%(y-x+1)+x;
}

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

} /* namespace tankwar */
