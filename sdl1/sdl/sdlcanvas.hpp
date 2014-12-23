#ifndef SDLCANVAS_H_
#define SDLCANVAS_H_

#include "options.hpp"
#include "projectile.hpp"
#include "background.hpp"
#include "canvas.hpp"
#include "ship.hpp"
#include "battlefieldlayout.hpp"

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
#include <deque>
#include <list>

namespace neurocid {
using std::string;
using std::map;
using std::list;

typedef gcn::Color Color;


class BattleField;
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

struct Trail : public std::deque<Vector2D> {
	Color color_;
};

class SDLCanvas : public Canvas {
	friend class Background;
private:
	struct SDL_Surface *screen_;
	Coord width_;
	Coord height_;
	Coord scale_;
	Coord zoom_;
	Rect viewPort_;
	Background background_;
	BattleFieldLayout bfl_;
	list<Explosion> explosions_;
	map<Ship*, Trail> trails_;
	Coord angle_ = M_PI / 4;

	void calculateScale();
	std::pair<Sint16, Sint16> scale(const Vector2D& v, Coord z);
	std::pair<Sint16, Sint16> transform(const Vector2D& v, Coord z = 1);

	Rect findBounds(BattleField& field);
	void drawBorder(BattleField& field);
	void drawGrid(BattleField& field);
	void drawStar(Star& s);
	void drawSurface(SDL_Surface *s, SDL_Rect& srect, Coord x, Coord y);
	void drawEllipse(Vector2D loc, Coord rangeX, Coord rangeY, Color c);
	void fillCircle(Vector2D loc, Coord radius, Color c);
	void drawExplosion(Explosion& expl);
	void drawTrail(const Trail& trail, const Color& c);
	void drawLine(Coord x0, Coord y0, Coord x1, Coord y1, Color c, Coord scale = 1);
	void drawRect(Coord x0, Coord y0, Coord x1, Coord y1, Color c, Coord s = 1);
	void fillRect(Coord x0, Coord y0, Coord x1, Coord y1, Color c, Coord s = 1);
	void drawShip(Ship& tank, Color c);
	void drawFacility(Facility& facility, Color c);
	void drawProjectile(Projectile& pro, Color& c);
	void drawCenters(Scanner& scanner);
	void pushTrail(Ship& t);
public:
	SDLCanvas(Coord width, Coord height, BattleFieldLayout& bfl);
	virtual ~SDLCanvas();

	virtual void zoomIn() override;
	virtual void zoomOut() override;
	virtual void left() override;
	virtual void right() override;
	virtual void up() override;
	virtual void down() override;
	virtual void tiltUp() override;
	virtual void tiltDown() override;

	virtual void update() override;
	virtual void clear() override;
	virtual void render(BattleField& field) override;
	virtual void reset() override;

	SDL_Surface* getSurface() const {
		return screen_;
	}

	Coord width() const override {
		return width_;
	}

	Coord height() const override {
		return height_;
	}
};
}

#endif /* SDLCANVAS_H_ */
