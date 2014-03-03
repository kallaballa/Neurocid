#ifndef CANVAS_H_
#define CANVAS_H_

#include <algorithm>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_image.h>
#include "2d.hpp"
#include <string>

namespace tankwar {
using std::string;

struct Color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

class BattleField;
class Tank;
class Projectile;
class Object;

class Canvas {
public:
  Canvas(Coord width, Coord height);
  void drawLine(Coord x0, Coord y0, Coord x1, Coord y1, Color& c);
  void drawTank(Tank& tank);
  void drawProjectile(Projectile& pro, Color& c);
  void drawExplosion(Object& o, Color& c);
  void update();
  void clear();
  void render(BattleField& field);

  void setTimeout(size_t millis) {
  	timeout_=millis;
  }

  size_t getTimeout() {
   	return timeout_;
   }


  bool isEnabled() {
	  return enabled_;
  }

  void setEnabled(bool e) {
	  enabled_ = e;
  }
private:
  class SDL_Surface *screen_;
  bool enabled_;
  Coord width_;
  Coord height_;
  size_t timeout_;
};
}

#endif /* CANVAS_H_ */
