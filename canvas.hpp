#ifndef CANVAS_H_
#define CANVAS_H_

#include <algorithm>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_image.h>
#include "2d.hpp"
#include <string>
#include <cassert>
#include "options.hpp"

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
private:
	static Canvas* instance_;
	Canvas(Coord width, Coord height);
	std::vector<Color> teamColors_ = {
			{0,255,0},
			{0,0,255},
			{0,255,255},
			{255,0,255},
			{255,255,0}
	};
public:
  void drawLine(Coord x0, Coord y0, Coord x1, Coord y1, Color& c);
  void drawTank(Tank& tank, Color& c);
  void drawProjectile(Projectile& pro, Color& c);
  void drawExplosion(Object& o, Color& c);
  void update();
  void clear();
  void render(BattleField& field);


  static Canvas* getInstance() {
	  if(instance_ == NULL)
		  instance_ = new Canvas(Options::getInstance()->WINDOW_WIDTH, Options::getInstance()->WINDOW_HEIGHT);

	  return instance_;
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
