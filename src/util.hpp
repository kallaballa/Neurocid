/*
 * util.hpp
 *
 *  Created on: Mar 2, 2014
 *      Author: elchaschab
 */

#ifndef UTIL_HPP_
#define UTIL_HPP_

namespace tankwar {

double fRand(double fMin, double fMax);
int iRand(int x,int y);

template<class T>
void clamp(T &arg, T min, T max)
{
  if (arg < min) {
    arg = min;
  }

  if (arg > max) {
    arg = max;
  }
}
} /* namespace tankwar */

#endif /* UTIL_HPP_ */
