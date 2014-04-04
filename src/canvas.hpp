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

namespace neurocid {
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
			{ 0, 192, 0 },
			{ 64, 64, 255 },
			{ 0, 255,255 },
			{ 255, 0, 255 },
			{ 255, 255, 0 }
	};

	struct SDL_Surface *screen_;
	void *frameBuffer_;
	TTF_Font *font_;
	bool drawEngines_;
	bool drawCenters_;
	bool drawGrid_;
	bool drawProjectiles_;
	Coord width_;
	Coord height_;
	Coord scale_;
	Coord zoom_;
	Rect viewPort_;
	map<string,string> osdMap_;

	void calculateScale();
	Sint16 scaleX(const Coord& c);
	Sint16 scaleY(const Coord& c);
	Rect findBounds(BattleField& field);
	void drawGrid(BattleField& field);
public:
	void zoomIn();
	void zoomOut();
	void left();
	void right();
	void up();
	void down();
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
	void render(BattleField& fiseld);

	SDL_Surface* getSurface() const {
		return screen_;
	}

	static Canvas* getInstance() {
		if (instance_ == NULL)
			instance_ = new Canvas(Options::getInstance()->WINDOW_WIDTH,
					Options::getInstance()->WINDOW_HEIGHT);

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

	void enableDrawProjectiles(bool e) {
		drawProjectiles_ = e;
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

	bool isDrawProjectilesEnabled() {
		return drawProjectiles_;
	}
};
}

#endif /* CANVAS_H_ */
