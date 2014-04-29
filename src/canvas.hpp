#ifndef CANVAS_H_
#define CANVAS_H_

#include "options.hpp"
#include "projectile.hpp"
#include "background.hpp"

#include <algorithm>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include "2d.hpp"
#include <string>
#include <cassert>
#include <map>
#include <list>

namespace neurocid {
using std::string;
using std::map;
using std::list;

typedef gcn::Color Color;

class BattleField;
class Ship;
class Facility;
class Object;

struct Explosion {
	Vector2D loc_;
	size_t tick_;
	size_t maxTick_;
	Color color_;

	size_t next() {
		assert(!this->end());
		return ++tick_;
	}

	bool end() {
		return tick_ >= maxTick_;
	}
};

class Canvas {
private:
	static Canvas* instance_;
	Canvas(Coord width, Coord height, BattleFieldLayout& bfl);
	~Canvas();

	struct SDL_Surface *screen_;
	bool drawEngines_;
	bool drawCenters_;
	bool drawGrid_;
	bool drawProjectiles_;
	bool drawElite_;

	Coord width_;
	Coord height_;
	Coord scale_;
	Coord zoom_;
	Rect viewPort_;
	Background background_;
	list<Explosion> explosions_;

	void calculateScale();
	Sint16 scaleX(const Coord& c, const Coord& scale);
	Sint16 scaleY(const Coord& c, const Coord& scale);
	Sint16 scaleX(const Coord& c);
	Sint16 scaleY(const Coord& c);
	Rect findBounds(BattleField& field);
public:
	void drawBorder(BattleField& field);
	void drawGrid(BattleField& field);
	void drawStar(Star& s);
	void drawSurface(SDL_Surface *s, SDL_Rect& srect, Coord x, Coord y);
	void drawEllipse(Vector2D loc, Coord rangeX, Coord rangeY, Color c);
	void fillCircle(Vector2D loc, Coord radius, Color c);
	void drawExplosion(Explosion& expl);
	void drawLine(Coord x0, Coord y0, Coord x1, Coord y1, Color& c);
	void drawShip(Ship& tank, Color c);
	void drawFacility(Facility& facility, Color c);
	void drawProjectile(Projectile& pro, Color& c);
	void drawCenters(Scanner& scanner);
	void zoomIn();
	void zoomOut();
	void left();
	void right();
	void up();
	void down();
	void update();
	void clear();
	void render(BattleField& fiseld);
	void reset();

	SDL_Surface* getSurface() const {
		return screen_;
	}

	Coord width() const {
		return width_;
	}

	Coord height() const {
		return height_;
	}

	static void init(BattleFieldLayout& bfl) {
		assert(instance_ == NULL);
		instance_ = new Canvas(Options::getInstance()->WINDOW_WIDTH,
				Options::getInstance()->WINDOW_HEIGHT, bfl);
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

	void enableDrawProjectiles(bool e) {
		drawProjectiles_ = e;
	}

	void enableDrawElite(bool e) {
		drawElite_ = e;
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

	bool isDrawEliteEnabled() {
		return drawElite_;
	}
};
}

#endif /* CANVAS_H_ */
