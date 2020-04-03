#ifndef FELGOGFX_HPP
#define FELGOGFX_HPP

#include <cstdint>
#include <QMetaObject>

namespace neurocid {

class FelgoGFX
{
private:
    QObject* canvas_;
public:
    FelgoGFX(QObject* canvas);

    void lineRGBA(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t r, int16_t g, int16_t b, int16_t a);
    void circleRGBA(int16_t x, int16_t y, int16_t radius, int16_t r, int16_t g, int16_t b, int16_t a);
    void filledCircleRGBA(int16_t x, int16_t y, int16_t radius, int16_t r, int16_t g, int16_t b, int16_t a);

    void clear();
    void flip();
};
}
#endif // FELGOGFX_HPP
