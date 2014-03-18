
#include "canvas.hpp"
#include "tank.hpp"
#include "projectile.hpp"
#include "battlefield.hpp"
#include "population.hpp"
#include <thread>
#include <chrono>
#include <iostream>


namespace tankwar {
using std::cerr;
using std::endl;

Canvas* Canvas::instance_ = NULL;

Canvas::Canvas(Coord width, Coord height) :
  screen_(NULL) , enabled_(true), width_(width), height_(height), timeout_(20) {
  if (width > 0 && height > 0) {
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
      cerr << "Can't init SDL: " << SDL_GetError() << endl;
      exit(1);
    }
    atexit(SDL_Quit);
    screen_ = SDL_SetVideoMode(width, height, 16, SDL_SWSURFACE);
    if (screen_ == NULL) {
      cerr << "Can't set video mode: " << SDL_GetError() << endl;
      exit(1);
    }
  }

  // Initialize SDL_ttf library
     if (TTF_Init() != 0)
     {
    	cerr << "TTF_Init() Failed: " << TTF_GetError() << endl;
        SDL_Quit();
        exit(1);
     }

     // Load a font
     font_ = TTF_OpenFont("/usr/share/fonts/truetype/DejaVuSerif.ttf", 24);
     if (font_ == NULL)
     {
        cerr << "TTF_OpenFont() Failed: " << TTF_GetError() << endl;
        TTF_Quit();
        SDL_Quit();
        exit(1);
     }
}

void Canvas::calculateScale() {
	Coord scaleX = width_ / (viewPort_.lr_.x - viewPort_.ul_.x);
	Coord scaleY = height_ / (viewPort_.lr_.y - viewPort_.ul_.y);
	scale_ = std::min(scaleX, scaleY);
}

Coord Canvas::scaleX(const Coord& c) {
	Coord preScale = 0.9;
	Coord len = (viewPort_.lr_.x - viewPort_.ul_.x) * scale_ * preScale;
	Coord pos = (c - viewPort_.ul_.x) * scale_ * preScale;
	Coord scaled = (((width_ - len) / 2) + pos);

	return round(scaled);
}

Coord Canvas::scaleY(const Coord& c) {
	Coord preScale = 0.9;
	Coord len = (viewPort_.lr_.y - viewPort_.ul_.y) * scale_ * preScale;
	Coord pos = (c - viewPort_.ul_.y) * scale_ * preScale;
	Coord scaled = (((height_ - len) / 2) + pos);

	return round(scaled);
}

void Canvas::drawEllipse(Vector2D loc, Coord rangeX, Coord rangeY, Color c) {
	ellipseRGBA(screen_, scaleX(loc.x), scaleY(loc.y), rangeX * scale_, rangeY * scale_ , c.r, c.g, c.b, 255);
}

void Canvas::drawLine(Coord x0, Coord y0, Coord x1, Coord y1, Color& c) {
    lineRGBA(screen_, scaleX(x0), scaleY(y0), scaleX(x1), scaleY(y1), c.r, c.g, c.b, 255);
}

void Canvas::drawText(const string& s, Coord x0, Coord y0, Color c) {
	SDL_Surface *text;
	SDL_Color text_color = { c.r, c.g, c.b };
	text = TTF_RenderText_Solid(font_, s.c_str(), text_color);
	assert(text != NULL);
	SDL_Rect drest = {scaleX(x0),scaleY(y0),0,0};
	if (SDL_BlitSurface(text, NULL, screen_, &drest) != 0) {
		cerr << "SDL_BlitSurface() Failed: " << SDL_GetError() << endl;
	}
	SDL_FreeSurface(text);
}

void Canvas::drawTank(Tank& tank, Color c) {
    Vector2D tip = tank.loc_;
    tip += tank.getDirection() * (tank.range_) * 5;

	drawEllipse(tank.loc_, tank.range_, tank.range_, c);
	drawLine(tank.loc_.x, tank.loc_.y, tip.x, tip.y ,c);
}

void Canvas::drawProjectile(Projectile& pro, Color& c) {
	drawEllipse(pro.loc_, pro.range_, pro.range_, c);
}

void Canvas::drawExplosion(Object& o, Color& c) {
	drawEllipse(o.loc_, 10, 10, c);
}

void Canvas::clear() {
	SDL_FillRect(screen_,NULL, 0x000000);
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
		for(Tank& t : team) {
			min.x = std::min(t.loc_.x, min.x);
			min.y = std::min(t.loc_.y, min.y);
			max.x = std::max(t.loc_.x, max.x);
			max.y = std::max(t.loc_.y, max.y);
		}
	}

	return {min, max};
}

void Canvas::drawGrid(BattleField& field) {
	Color grey = {64,64,64};
	Color darkred = {127,0,0};

	for(Coord x = 0; x < field.layout_.width_; x+=1000) {
		drawLine(x,0,x, field.layout_.height_, grey);
	}
	for(Coord y = 0; y < field.layout_.height_; y+=1000) {
		drawLine(0,y,field.layout_.width_,y, grey);
	}

	drawEllipse(Vector2D(field.layout_.width_/2, field.layout_.height_/2), 20, 20, darkred);
	drawLine(0,0,0, field.layout_.height_, darkred);
	drawLine(0,0,field.layout_.width_, 0, darkred);
	drawLine(field.layout_.width_,field.layout_.height_, field.layout_.width_,0, darkred);
	drawLine(field.layout_.width_,field.layout_.height_, 0,field.layout_.height_, darkred);
}

void Canvas::render(BattleField& field) {
	assert(field.teams_.size() == 2);
	assert(teamColors_.size() >= field.teams_.size());
	viewPort_ = findBounds(field);
	calculateScale();
	this->clear();
	drawGrid(field);

	Color white = {255,255,255};
	Color red = {255,0,0};

	size_t teamCnt = 0;
	for(Population& team : field.teams_) {
		for(Tank& t : team) {
			if(t.explode_)
				this->drawExplosion(t, red);
			else if(!t.dead_)
				this->drawTank(t, teamColors_[teamCnt]);
			t.explode_ = false;

			for(Projectile* p : t.projectiles_) {
				if(p->explode_)
					this->drawExplosion(*p, red);
				else if(!p->dead_)
					this->drawProjectile(*p,white);
				p->explode_ = false;
			}
		}
		++teamCnt;
	}

	drawText(std::to_string(field.teams_[0].score_), 50, 50, teamColors_[0]);
	drawText(std::to_string(field.teams_[1].score_), width_ - 70, 50, teamColors_[1]);

	this->update();
}
}
