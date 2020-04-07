#include "felgogfx.hpp"

#include <QVariant>
#include <iostream>
#include <iterator>

namespace neurocid {

FelgoGFX::FelgoGFX(QObject* canvas) : canvas_(canvas) {
}

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

//FIXME test me
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

void FelgoGFX::ellipseRGBA(const int16_t& x, const int16_t& y, const int16_t& radiusx, const int16_t& radiusy, const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a) {
  QMetaObject::invokeMethod(canvas_, "ellipseRGBA",
                            Q_ARG(QVariant, x),
                            Q_ARG(QVariant, y),
                            Q_ARG(QVariant, radiusx),
                            Q_ARG(QVariant, radiusy),
                            Q_ARG(QVariant, r),
                            Q_ARG(QVariant, g),
                            Q_ARG(QVariant, b),
                            Q_ARG(QVariant, a)
                            );
}

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

void FelgoGFX::clear(const uint8_t& r, const uint8_t& g, const uint8_t& b) {
  QMetaObject::invokeMethod(canvas_, "clear",
                            Q_ARG(QVariant, r),
                            Q_ARG(QVariant, g),
                            Q_ARG(QVariant, b)
                            );
}

void FelgoGFX::flip() {
  QMetaObject::invokeMethod(canvas_, "flip");
}

}
