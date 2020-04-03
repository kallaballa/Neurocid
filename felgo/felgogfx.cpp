#include "felgogfx.hpp"

#include <QVariant>
#include <iostream>

namespace neurocid {

FelgoGFX::FelgoGFX(QObject* canvas) : canvas_(canvas) {

}


void FelgoGFX::lineRGBA(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t r, int16_t g, int16_t b, int16_t a) {
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


void FelgoGFX::circleRGBA(int16_t x, int16_t y, int16_t radius, int16_t r, int16_t g, int16_t b, int16_t a) {
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

void FelgoGFX::filledCircleRGBA(int16_t x, int16_t y, int16_t radius, int16_t r, int16_t g, int16_t b, int16_t a) {
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

void FelgoGFX::clear() {
  QMetaObject::invokeMethod(canvas_, "clear");
}

void FelgoGFX::flip() {
  QMetaObject::invokeMethod(canvas_, "flip");
}

}
