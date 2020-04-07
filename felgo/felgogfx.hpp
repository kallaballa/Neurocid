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
    /**
        except for the return value this function behaves like explained here:
        https://www.ferzkopp.net/Software/SDL_gfx-2.0/Docs/html/_s_d_l__gfx_primitives_8h.html#a5e4bd13b12d34698fbcb2dc9d3a0e9f3
    */
    void lineRGBA(const int16_t& x1, const int16_t& y1, const int16_t& x2, const int16_t& y2, const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a);
    /**
        except for the return value this function behaves like explained here:
        https://www.ferzkopp.net/Software/SDL_gfx-2.0/Docs/html/_s_d_l__gfx_primitives_8h.html#a5e4bd13b12d34698fbcb2dc9d3a0e9f3
    */
    void circleRGBA(const int16_t& x, const int16_t& y, const int16_t& radius, const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a);
    /**
        except for the return value this function behaves like explained here:
        https://www.ferzkopp.net/Software/SDL_gfx-2.0/Docs/html/_s_d_l__gfx_primitives_8h.html#a8e0945b74c02cdb1441e1b2a29d2c87d
    */
    void filledCircleRGBA(const int16_t& x, const int16_t& y, const int16_t& radius, const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a);
    /**
        except for the return value this function behaves like explained here:
        https://www.ferzkopp.net/Software/SDL_gfx-2.0/Docs/html/_s_d_l__gfx_primitives_8h.html#a8e0945b74c02cdb1441e1b2a29d2c87d
    */
    void pieRGBA(const int16_t& x, const int16_t& y, const int16_t& radius, const int16_t& start, const int16_t& end, const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a);
    /**
        except for the return value this function behaves like explained here:
        https://www.ferzkopp.net/Software/SDL_gfx-2.0/Docs/html/_s_d_l__gfx_primitives_8h.html#a2b1023ddbbb25d57bd51676b49234af4
    */
    void filledPolygonRGBA(const int16_t* vx,	const int16_t* vy, int n, const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a);
    /**
        except for the return value this function behaves like explained here:
        https://www.ferzkopp.net/Software/SDL_gfx-2.0/Docs/html/_s_d_l__gfx_primitives_8h.html#a2b1023ddbbb25d57bd51676b49234af4
    */
    void ellipseRGBA(const int16_t& x, const int16_t& y, const int16_t& radiusx, const int16_t& radiusy, const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a);
    /**
        except for the return value this function behaves like explained here:
        https://www.ferzkopp.net/Software/SDL_gfx-2.0/Docs/html/_s_d_l__gfx_primitives_8h.html#a2b1023ddbbb25d57bd51676b49234af4
    */
    void rectangleRGBA(const int16_t& x1, const int16_t& y1, const int16_t& x2, const int16_t& y2, const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a);
    /**
        except for the return value this function behaves like explained here:
        https://www.ferzkopp.net/Software/SDL_gfx-2.0/Docs/html/_s_d_l__gfx_primitives_8h.html#a2b1023ddbbb25d57bd51676b49234af4
    */
    void clear(const uint8_t& r, const uint8_t& g, const uint8_t& b);
    /**
        This function has no pendant in SDL_gfx but it serves the same purpose as SDL_Flip, though
        the exact semantics are very different.
    */
    void flip();
};
}
#endif // FELGOGFX_HPP
