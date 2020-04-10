#include "felgogfx.hpp"

#include <QVariant>
#include <iostream>
#include <iterator>

/*!
    \class neurocid::FelgoGfx
    \brief A class that simply forwards SDL_gfx-like primitive calls to the QML \c GfxCanvas
*/
namespace neurocid {

FelgoGFX::FelgoGFX(QObject* canvas) : canvas_(canvas) {
}

/*!
 * \brief FelgoGFX::lineRGBA
 * \param x1
 * \param y1
 * \param x2
 * \param y2
 * \param r
 * \param g
 * \param b
 * \param a
 *  except for the return value this function behaves like explained here:
 *  https://www.ferzkopp.net/Software/SDL_gfx-2.0/Docs/html/_s_d_l__gfx_primitives_8h.html#a5e4bd13b12d34698fbcb2dc9d3a0e9f3
 */
void FelgoGFX::lineRGBA(const int16_t& x1, const int16_t& y1, const int16_t& x2, const int16_t& y2, const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a) {
    QMetaObject::invokeMethod(canvas_, "lineRGBA",
                              Q_ARG(QVariant, x1),
                              Q_ARG(QVariant, y1),
                              Q_ARG(QVariant, x2),
                              Q_ARG(QVariant, y2),
                              Q_ARG(QVariant, r),
                              Q_ARG(QVariant, g),
                              Q_ARG(QVariant, b),
                              Q_ARG(QVariant, a)
                              );
}

/*!
 * \brief FelgoGFX::circleRGBA
 * \param x
 * \param y
 * \param radius
 * \param r
 * \param g
 * \param b
 * \param a
 * except for the return value this function behaves like explained here:
 * https://www.ferzkopp.net/Software/SDL_gfx-2.0/Docs/html/_s_d_l__gfx_primitives_8h.html#a5e4bd13b12d34698fbcb2dc9d3a0e9f3
 */
void FelgoGFX::circleRGBA(const int16_t& x, const int16_t& y, const int16_t& radius, const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a) {
    QMetaObject::invokeMethod(canvas_, "circleRGBA",
                              Q_ARG(QVariant, x),
                              Q_ARG(QVariant, y),
                              Q_ARG(QVariant, radius),
                              Q_ARG(QVariant, r),
                              Q_ARG(QVariant, g),
                              Q_ARG(QVariant, b),
                              Q_ARG(QVariant, a)
                              );
}

/*!
 * \brief FelgoGFX::filledCircleRGBA
 * \param x
 * \param y
 * \param radius
 * \param r
 * \param g
 * \param b
 * \param a
 * except for the return value this function behaves like explained here:
 * https://www.ferzkopp.net/Software/SDL_gfx-2.0/Docs/html/_s_d_l__gfx_primitives_8h.html#a8e0945b74c02cdb1441e1b2a29d2c87d
 */
void FelgoGFX::filledCircleRGBA(const int16_t& x, const int16_t& y, const int16_t& radius, const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a) {
    QMetaObject::invokeMethod(canvas_, "filledCircleRGBA",
                              Q_ARG(QVariant, x),
                              Q_ARG(QVariant, y),
                              Q_ARG(QVariant, radius),
                              Q_ARG(QVariant, r),
                              Q_ARG(QVariant, g),
                              Q_ARG(QVariant, b),
                              Q_ARG(QVariant, a)
                              );
}

/*!
 * \brief FelgoGFX::pieRGBA
 * \param x
 * \param y
 * \param radius
 * \param start
 * \param end
 * \param r
 * \param g
 * \param b
 * \param a
 * except for the return value this function behaves like explained here:
 * https://www.ferzkopp.net/Software/SDL_gfx-2.0/Docs/html/_s_d_l__gfx_primitives_8h.html#a8e0945b74c02cdb1441e1b2a29d2c87d
 */
void FelgoGFX::pieRGBA(const int16_t& x, const int16_t& y, const int16_t& radius, const int16_t& start, const int16_t& end, const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a) {
    QMetaObject::invokeMethod(canvas_, "pieRGBA",
                              Q_ARG(QVariant, x),
                              Q_ARG(QVariant, y),
                              Q_ARG(QVariant, radius),
                              Q_ARG(QVariant, start),
                              Q_ARG(QVariant, end),
                              Q_ARG(QVariant, r),
                              Q_ARG(QVariant, g),
                              Q_ARG(QVariant, b),
                              Q_ARG(QVariant, a)
                              );
}

/*!
 * \brief FelgoGFX::filledPolygonRGBA
 * \param vx
 * \param vy
 * \param n
 * \param r
 * \param g
 * \param b
 * \param a
 * except for the return value this function behaves like explained here:
 * https://www.ferzkopp.net/Software/SDL_gfx-2.0/Docs/html/_s_d_l__gfx_primitives_8h.html#a2b1023ddbbb25d57bd51676b49234af4
 */
void FelgoGFX::filledPolygonRGBA(const int16_t* vx,	const int16_t* vy, int n, const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a) {
	QVariantList lx;
  QVariantList ly;
	lx.reserve(n);
	ly.reserve(n);
	std::copy(vx + 0, vx + n, std::back_inserter(lx));
	std::copy(vy + 0, vy + n, std::back_inserter(ly));

	QMetaObject::invokeMethod(canvas_, "filledPolygonRGBA",
                                Q_ARG(QVariant, QVariant::fromValue(lx)),
                                Q_ARG(QVariant, QVariant::fromValue(ly)),
	                              Q_ARG(QVariant, r),
	                              Q_ARG(QVariant, g),
	                              Q_ARG(QVariant, b),
	                              Q_ARG(QVariant, a)
	                              );
}

/*!
 * \brief FelgoGFX::ellipseRGBA
 * \param x
 * \param y
 * \param radiusx
 * \param radiusy
 * \param r
 * \param g
 * \param b
 * \param a
 *  except for the return value this function behaves like explained here:
 *  https://www.ferzkopp.net/Software/SDL_gfx-2.0/Docs/html/_s_d_l__gfx_primitives_8h.html#a2b1023ddbbb25d57bd51676b49234af4
 */
void FelgoGFX::ellipseRGBA(const int16_t& x, const int16_t& y, const int16_t& radiusx, const int16_t& radiusy, const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a) {
  QMetaObject::invokeMethod(canvas_, "ellipseRGBA",
                            Q_ARG(QVariant, x),
                            Q_ARG(QVariant, radiusx),
                            Q_ARG(QVariant, y),
                            Q_ARG(QVariant, radiusy),
                            Q_ARG(QVariant, r),
                            Q_ARG(QVariant, g),
                            Q_ARG(QVariant, b),
                            Q_ARG(QVariant, a)
                            );
}

/*!
 * \brief FelgoGFX::rectangleRGBA
 * \param x1
 * \param y1
 * \param x2
 * \param y2
 * \param r
 * \param g
 * \param b
 * \param a
 * except for the return value this function behaves like explained here:
 * https://www.ferzkopp.net/Software/SDL_gfx-2.0/Docs/html/_s_d_l__gfx_primitives_8h.html#a2b1023ddbbb25d57bd51676b49234af4
 */
void FelgoGFX::rectangleRGBA(const int16_t& x1, const int16_t& y1, const int16_t& x2, const int16_t& y2, const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a) {
  QMetaObject::invokeMethod(canvas_, "rectangleRGBA",
                            Q_ARG(QVariant, x1),
                            Q_ARG(QVariant, y1),
                            Q_ARG(QVariant, x2),
                            Q_ARG(QVariant, y2),
                            Q_ARG(QVariant, r),
                            Q_ARG(QVariant, g),
                            Q_ARG(QVariant, b),
                            Q_ARG(QVariant, a)
                            );
}


/*!
 * \brief FelgoGFX::clear
 * \param r
 * \param g
 * \param b
 * This function has no corresponding function in SDL_gfx. It fills the whole canvas with a uniform color.
 */

void FelgoGFX::clear(const uint8_t& r, const uint8_t& g, const uint8_t& b) {
  QMetaObject::invokeMethod(canvas_, "clear",
                            Q_ARG(QVariant, r),
                            Q_ARG(QVariant, g),
                            Q_ARG(QVariant, b)
                            );
}

/*!
 * \brief FelgoGFX::flip
 *  This function has no corresponding function in SDL_gfx but it serves the same purpose as SDL_Flip, though
 *  the exact semantics are very different.
 */
void FelgoGFX::flip() {
  QMetaObject::invokeMethod(canvas_, "flip");
}

}
