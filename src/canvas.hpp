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
	std::vector<Color> teamColors_ = {
			{ 0, 192, 0 },
			{ 64, 64, 255 },
			{ 0, 255,255 },
			{ 255, 0, 255 },
			{ 255, 255, 0 }
	};

public:
	bool drawEngines_;
	bool drawCenters_;
	bool drawGrid_;

	virtual void render(BattleField* field) = 0;

	static void init(Canvas* instance) {
		assert(instance_ == NULL);
		instance_ = instance;
	}

	static Canvas* getInstance() {
		assert(instance_ != NULL);
		return instance_;
	}

	void enableDrawCenters(bool e) {
		drawCenters_ = e;
	}

	void enableDrawEngines(bool e) {
		drawEngines_ = e;
	}

	void enableDrawGrid(bool e) {
		drawGrid_ = e;
	}

	bool isDrawCentersEnabled() {
		return drawCenters_;
	}

	bool isDrawEnginesEnabled() {
		return drawEngines_;
	}

	bool isDrawGridEnabled() {
		return drawGrid_;
	}
};

class SDLCanvas : public Canvas {
private:
	std::vector<Color> teamColors_ = {
			{ 0, 192, 0 },
			{ 64, 64, 255 },
			{ 0, 255,255 },
			{ 255, 0, 255 },
			{ 255, 255, 0 }
	};

	struct SDL_Surface *screen_;
	TTF_Font *font_;
	Coord width_;
	Coord height_;
	Coord scale_;
	Rect viewPort_;
	map<string,string> osdMap_;

	void calculateScale();
	Sint16 scaleX(const Coord& c);
	Sint16 scaleY(const Coord& c);

	Rect findBounds(BattleField& field);
	void drawGrid(BattleField& field);
	void drawEllipse(Vector2D loc, Coord rangeX, Coord rangeY, Color c);
	void drawText(const string& s, Coord x0, Coord y0, Color c);
	void updateOSD(const string& key, const string& value);
	void renderOSD();
	Coord renderText(const string& s, Coord x, Color c, bool left);
	void renderTackerInfo();
	void drawLine(Coord x0, Coord y0, Coord x1, Coord y1, Color& c);
	void drawTank(Tank& tank, Color c);
	void drawProjectile(Projectile& pro, Color& c);
	void drawExplosion(Object& o, Color& c);
	void drawCenters(Scanner& scanner);
	void update();
	void clear();
public:
	SDLCanvas(Coord width, Coord height);
	virtual void render(BattleField* field);
};
}

#endif /* CANVAS_H_ */
