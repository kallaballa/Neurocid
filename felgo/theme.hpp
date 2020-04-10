/*
 * theme.hpp
 *
 *  Created on: Apr 11, 2014
 *      Author: elchaschab
 */

#ifndef THEME_HPP_
#define THEME_HPP_

#include "color.hpp"

namespace neurocid {

typedef gcn::Color Color;

class Theme {
public:
    static Color teamA;
    static Color teamB;

    static Color projectileA;
    static Color projectileB;

    static Color enginesA;
    static Color enginesB;

    static Color explosion;

    static Color battleFieldCenter;
    static Color battleFieldBorder;
    static Color battleFieldGrid;

    static Color osdWidgetBg;
    static Color osdWidgetFg;
    static Color osdWidgetFrameMid;
    static Color osdWidgetFrameOutter;
    static Color buttonBg;
    static Color buttonFg;
    static Color labelBg;
    static Color labelFg;
};

} /* namespace neurocid */

#endif /* THEME_HPP_ */
