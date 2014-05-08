#ifndef SDLCANVAS_H_
#define SDLCANVAS_H_

#include "options.hpp"
#include "projectile.hpp"
#include "background.hpp"
#include "canvas.hpp"

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
	list<Explosion> explosions_;

	void calculateScale();
	Sint16 scaleX(const Coord& c, const Coord& scale);
	Sint16 scaleY(const Coord& c, const Coord& scale);
	Sint16 scaleX(const Coord& c);
	Sint16 scaleY(const Coord& c);
	Rect findBounds(BattleField& field);
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
public:
	SDLCanvas(Coord width, Coord height, BattleFieldLayout& bfl);
	virtual ~SDLCanvas();

	virtual void zoomIn() override;
	virtual void zoomOut() override;
	virtual void left() override;
	virtual void right() override;
	virtual void up() override;
	virtual void down() override;

	virtual void update() override;
	virtual void clear() override;
	virtual void render(BattleField& field) override;
	virtual void reset() override;

	SDL_Surface* getSurface() const override {
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
