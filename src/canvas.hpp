#ifndef CANVAS_H_
#define CANVAS_H_

#include <algorithm>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "2d.hpp"
#include <string>
#include <cassert>
#include <map>
#include "options.hpp"
#include "projectile.hpp"

namespace tankwar {
using std::string;
using std::map;

struct Color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

class BattleField;
class Tank;
class Object;

class Canvas {
private:
	static Canvas* instance_;
	Canvas(Coord width, Coord height);
	std::vector<Color> teamColors_ = {
			{ 0, 255, 0 },
			{ 0, 0, 255 },
			{ 0, 255,255 },
			{ 255, 0, 255 },
			{ 255, 255, 0 }
	};

	class SDL_Surface *screen_;
	TTF_Font *font_;
	bool enabled_;
	Coord width_;
	Coord height_;
	size_t timeout_;
	Rect viewPort_;
	Coord scale_;
	map<string,string> osdMap_;

	void calculateScale();
	Coord scaleX(const Coord& c);
	Coord scaleY(const Coord& c);

	Rect findBounds(BattleField& field);
	void drawGrid(BattleField& field);
public:
	void drawEllipse(Vector2D loc, Coord rangeX, Coord rangeY, Color c);
	void drawText(const string& s, Coord x0, Coord y0, Color c);
	void updateOSD(const string& key, const string& value);
	void renderOSD();
	void drawLine(Coord x0, Coord y0, Coord x1, Coord y1, Color& c);
	void drawTank(Tank& tank, Color c);
	void drawProjectile(Projectile& pro, Color& c);
	void drawExplosion(Object& o, Color& c);
	void update();
	void clear();
	void render(BattleField& fiseld);

	static Canvas* getInstance() {
		if (instance_ == NULL)
			instance_ = new Canvas(Options::getInstance()->WINDOW_WIDTH,
					Options::getInstance()->WINDOW_HEIGHT);

		return instance_;
	}
};
}

#endif /* CANVAS_H_ */
