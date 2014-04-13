
#include "canvas.hpp"
#include "gui/gui.hpp"
#include "gui/osd.hpp"
#include "ship.hpp"
#include "projectile.hpp"
#include "battlefield.hpp"
#include "population.hpp"
#include "time_tracker.hpp"
#include "gui/theme.hpp"

#include <sstream>
#include <thread>
#include <chrono>
#include <iostream>
#include <algorithm>

namespace neurocid {
using std::stringstream;
using std::cerr;
using std::endl;

Canvas* Canvas::instance_ = NULL;

Canvas::Canvas(Coord width, Coord height) :
		screen_(NULL),
		drawEngines_(false),
		drawCenters_(false),
		drawGrid_(false),
		drawProjectiles_(true),
		drawElite_(false),
		width_(width),
		height_(height),
		scale_(1),
		zoom_(1),
		viewPort_(),
		background_(this) {
	if (width > 0 && height > 0) {
		if (SDL_Init(SDL_INIT_VIDEO) == -1) {
			cerr << "Can't init SDL: " << SDL_GetError() << endl;
			exit(1);
		}
		atexit(SDL_Quit);
		SDL_WM_SetCaption("NN SpaceShip Simulation", NULL);
		screen_ = SDL_SetVideoMode(width, height, 16, SDL_SWSURFACE);
		if (screen_ == NULL) {
			cerr << "Can't set video mode: " << SDL_GetError() << endl;
			exit(1);
		}
	}

	background_.init();
}

Canvas::~Canvas() {
}

void Canvas::calculateScale() {
	Coord preScale = 0.9;
	Coord scaleX = width_ / (viewPort_.lr_.x_ - viewPort_.ul_.x_);
	Coord scaleY = height_ / (viewPort_.lr_.y_ - viewPort_.ul_.y_);
	scale_ = std::min(scaleX, scaleY) * preScale / zoom_;
}

Sint16 Canvas::scaleX(const Coord& c, const Coord& scale) {
	Coord len = (viewPort_.lr_.x_ - viewPort_.ul_.x_) * scale;
	Coord pos = (c - viewPort_.ul_.x_) * scale;
	Coord scaled = (((width_ - len) / 2) + pos);

	return (Sint16)round(scaled);
}

Sint16 Canvas::scaleY(const Coord& c, const Coord& scale) {
	Coord len = (viewPort_.lr_.y_ - viewPort_.ul_.y_) * scale;
	Coord pos = (c - viewPort_.ul_.y_) * scale;
	Coord scaled = (((height_ - len) / 2) + pos);

	return (Sint16)round(scaled);
}

Sint16 Canvas::scaleX(const Coord& c) {
	return scaleX(c, scale_);
}

Sint16 Canvas::scaleY(const Coord& c) {
	return scaleY(c, scale_);
}

void Canvas::zoomIn() {
	if(zoom_ > 0.1)
		zoom_ -= 0.1;
	 else
		zoom_ -= 0.01;

	zoom_ = std::max(zoom_, 0.01);
}

void Canvas::zoomOut() {
	if(zoom_ < 0.1)
		zoom_ += 0.01;
	 else
		zoom_ += 0.1;

	zoom_ = std::min(zoom_, 1.0);
}

void Canvas::left() {
	viewPort_.ul_.x_ -= (2000 * zoom_);
	viewPort_.lr_.x_ -= (2000 * zoom_);
}

void Canvas::right() {
	viewPort_.ul_.x_ += (2000 * zoom_);
	viewPort_.lr_.x_ += (2000 * zoom_);
}

void Canvas::up() {
	viewPort_.ul_.y_ -= (2000 * zoom_);
	viewPort_.lr_.y_ -= (2000 * zoom_);
}

void Canvas::down() {
	viewPort_.ul_.y_ += (2000 * zoom_);
	viewPort_.lr_.y_ += (2000 * zoom_);
}

void Canvas::drawStar(Star& s) {
	double alpha = s.alpha;
	double lastR = 0;
	for(size_t i = 0; i < s.radius; i++) {
		if(s.scale == 0)
			circleRGBA(screen_, Sint16(s.x), Sint16(s.y), i, s.r,s.g,s.b, round(alpha));
		else {
			double r = round(i * scale_ * s.scale);
			if(r != lastR)
				circleRGBA(screen_, scaleX(s.x, scale_ * s.scale), scaleY(s.y, scale_ * s.scale), r, s.r,s.g,s.b, round(alpha));
			lastR = r;
		}
		if(i == s.discontinuity)
			alpha /= 3;
		alpha *= s.step;
	}
}

void Canvas::drawSurface(SDL_Surface *s, SDL_Rect& srect, Coord x, Coord y) {
	SDL_Rect drect = {scaleX(x), scaleY(y), 0, 0};
	SDL_BlitSurface(s, &srect, screen_, &drect);
}

void Canvas::drawEllipse(Vector2D loc, Coord rangeX, Coord rangeY, Color c) {
	ellipseRGBA(screen_, scaleX(loc.x_), scaleY(loc.y_), round(rangeX * scale_), round(rangeY * scale_), c.r, c.g, c.b, c.a);
}

void Canvas::drawLine(Coord x0, Coord y0, Coord x1, Coord y1, Color& c) {
    lineRGBA(screen_, scaleX(x0), scaleY(y0), scaleX(x1), scaleY(y1), c.r, c.g, c.b, c.a);
}

void Canvas::drawShip(Ship& ship, Color c) {
	Vector2D dir = ship.getDirection();
	Vector2D tip = ship.loc_;
    tip += dir * (ship.range_) * 5;

    drawLine(ship.loc_.x_, ship.loc_.y_, tip.x_, tip.y_ ,c);
    drawEllipse(ship.loc_, ship.range_, ship.range_, c);
	drawLine(ship.loc_.x_, ship.loc_.y_, tip.x_, tip.y_ ,c);
	if(drawElite_ && ship.isElite) {
	    drawEllipse(ship.loc_, 200, 200, {255,0,255});
	}

	if(drawEngines_) {
		Vector2D across1 = dir;
		Vector2D across2 = dir;
		across1.rotate(-45);
		across2.rotate(45);

		Vector2D wc = ship.loc_;
		Vector2D flengine = wc;
		Vector2D frengine = wc;
		Vector2D blengine = wc;
		Vector2D brengine = wc;

		flengine += (across1 * (ship.range_));
		frengine += (across2 * (ship.range_));
		blengine += (across2 * -(ship.range_));
		brengine += (across1 * -(ship.range_));

		Vector2D flforce = flengine;
		flforce += across2 * -(ship.flthrust_ * 600);
		Vector2D frforce = frengine;
		frforce += across1 * (ship.frthrust_ * 600);
		Vector2D blforce = blengine;
		blforce += across1 * (ship.blthrust_ * 600);
		Vector2D brforce = brengine;
		brforce += across2 * -(ship.brthrust_ * 600);

		Color cengine;
		if(ship.teamID_ == 0)
			cengine = Theme::enginesA;
		else
			cengine = Theme::enginesB;

		drawLine(flengine.x_, flengine.y_, flforce.x_, flforce.y_, cengine);
		drawLine(frengine.x_, frengine.y_, frforce.x_, frforce.y_, cengine);
		drawLine(blengine.x_, blengine.y_, blforce.x_, blforce.y_, cengine);
		drawLine(brengine.x_, brengine.y_, brforce.x_, brforce.y_, cengine);
	}
}

void Canvas::drawProjectile(Projectile& pro, Color& c) {
	if(!drawProjectiles_)
		return;

	Vector2D trail = pro.loc_;
	trail += pro.getDirection() * -500;
	drawLine(trail.x_,trail.y_, pro.loc_.x_, pro.loc_.y_, c);
}

void Canvas::drawExplosion(Object& o, Color& c) {
	drawEllipse(o.loc_, 10, 10, c);
}

void Canvas::clear() {
	SDL_Rect clrDest = {0,0, Uint16(width_), Uint16(height_)};
	SDL_FillRect(screen_, &clrDest, 0);
	background_.draw(screen_);
}

void Canvas::update() {
	if(screen_ != NULL) {
		SDL_Flip(screen_);
	}
}

Rect Canvas::findBounds(BattleField& field) {
	Vector2D min(std::numeric_limits<Coord>().max(), std::numeric_limits<Coord>().max());
	Vector2D max(0,0);

	for(Population& team : field.teams_) {
		for(Ship& t : team) {
			if(t.dead_)
				continue;
			min.x_ = std::min(t.loc_.x_, min.x_);
			min.y_ = std::min(t.loc_.y_, min.y_);
			max.x_ = std::max(t.loc_.x_, max.x_);
			max.y_ = std::max(t.loc_.y_, max.y_);
		}
	}

	return {min, max};
}

void Canvas::drawBorder(BattleField& field) {
	drawLine(0,0,0, field.layout_.height_, Theme::battleFieldBorder);
	drawLine(0,0,field.layout_.width_, 0, Theme::battleFieldBorder);
	drawLine(field.layout_.width_,field.layout_.height_, field.layout_.width_,0, Theme::battleFieldBorder);
	drawLine(field.layout_.width_,field.layout_.height_, 0,field.layout_.height_, Theme::battleFieldBorder);
}

void Canvas::drawGrid(BattleField& field) {
	for(Coord x = 0; x < field.layout_.width_; x+=5000) {
		drawLine(x,0,x, field.layout_.height_, Theme::battleFieldGrid);
	}
	for(Coord y = 0; y < field.layout_.height_; y+=5000) {
		drawLine(0,y,field.layout_.width_,y, Theme::battleFieldGrid);
	}

	drawEllipse(Vector2D(field.layout_.width_/2, field.layout_.height_/2), 20, 20, Theme::battleFieldCenter);
}

void Canvas::drawCenters(Scanner& scanner) {
	for(Vector2D center : scanner.centersA_) {
		drawEllipse(center, 3/ scale_, 3/ scale_, Theme::teamA);
		drawEllipse(center, 10/ scale_, 10/ scale_, Theme::teamA);
	}

	for(Vector2D center : scanner.centersB_) {
		drawEllipse(center, 3/ scale_, 3/ scale_, Theme::teamB);
		drawEllipse(center, 10/ scale_, 10/ scale_, Theme::teamB);
	}
}

void Canvas::render(BattleField& field) {
	assert(field.teams_.size() == 2);
	if(zoom_ == 1)
		viewPort_ = findBounds(field);
	calculateScale();

	drawBorder(field);

	if(drawGrid_)
		drawGrid(field);

	size_t teamCnt = 0;
	for(Population& team : field.teams_) {
		for(Ship& t : team) {
			if(t.explode_)
				this->drawExplosion(t, Theme::explosion);
			else if(!t.dead_) {
				if(t.teamID_ == 0 )
					this->drawShip(t, Theme::teamA);
				else
					this->drawShip(t, Theme::teamB);
			}
			t.explode_ = false;

			for(Projectile* p : t.projectiles_) {
				if(p->explode_)
					this->drawExplosion(*p, Theme::explosion);
				else if(!p->dead_) {
					if(t.teamID_ == 0)
						this->drawProjectile(*p,Theme::projectileA);
					else
						this->drawProjectile(*p,Theme::projectileB);
				}
				p->explode_ = false;
			}
		}
		++teamCnt;
	}

	if(drawCenters_)
		drawCenters(field.scanner_);
}
}
