/*
 * theme.cpp
 *
 *  Created on: Apr 11, 2014
 *      Author: elchaschab
 */

#include "theme.hpp"

namespace neurocid {
Color Theme::teamA = {243,80,80,255};
Color Theme::teamB = {80,243,80,255};

Color Theme::projectileA = {243,243,21,255};
Color Theme::projectileB = {21, 243, 243,255};

Color Theme::enginesA = {255,0,0,255};
Color Theme::enginesB = {255,0,0,255};

Color Theme::explosion = {255,0,0,255};

Color Theme::battleFieldCenter = {100,100,243,255};
Color Theme::battleFieldBorder = {100,100,243,255};
Color Theme::battleFieldGrid = {32,32,32,255};

Color Theme::osdWidgetBg = {100,100,243,150};
Color Theme::osdWidgetFg = {255,255,255,255};
Color Theme::osdWidgetFrameMid = {0,0,0,255};
Color Theme::osdWidgetFrameOutter = {255,255,255,255};

Color Theme::buttonBg = {80,80,243,150};
Color Theme::buttonFg = {255,255,255,255};

Color Theme::labelBg = {80,80,243,150};
Color Theme::labelFg = {255,255,255,255};

} /* namespace neurocid */
