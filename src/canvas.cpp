
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

void Canvas::drawText(const string& s, Coord x0, Coord y0, Color c) {
	SDL_Surface *text;
	SDL_Color text_color = { c.r, c.g, c.b };
	text = TTF_RenderText_Solid(font_, s.c_str(), text_color);
	assert(text != NULL);
	SDL_Rect drest = {x0,y0,0,0};
	if (SDL_BlitSurface(text, NULL, screen_, &drest) != 0) {
		cerr << "SDL_BlitSurface() Failed: " << SDL_GetError() << endl;
	}
	SDL_FreeSurface(text);
}

void Canvas::drawLine(Coord x0, Coord y0, Coord x1, Coord y1, Color& c) {
  if(screen_ != NULL) {
    lineRGBA(screen_, x0, y0, x1, y1, c.r, c.g, c.b, 128);
  }
}

void Canvas::drawTank(Tank& tank, Color c) {
    ellipseRGBA(screen_, (Uint16)tank.loc_.x, (Uint16)tank.loc_.y, (Uint16)tank.range_, (Uint16)tank.range_, c.r, c.g, c.b, 255);
    Vector2D tip = tank.loc_;
    tip += tank.dir_ * (tank.range_);
    ellipseRGBA(screen_, (Uint16)tip.x, (Uint16)tip.y, 2, 2, c.r, c.g, c.b, 255);
}

void Canvas::drawProjectile(Projectile& pro, Color& c) {
    ellipseRGBA(screen_, (Uint16)pro.loc_.x, (Uint16)pro.loc_.y, (Uint16)pro.range_, (Uint16)pro.range_, c.r, c.g, c.b, 255);
}

void Canvas::drawExplosion(Object& o, Color& c) {
	 ellipseRGBA(screen_, (Uint16)o.loc_.x, (Uint16)o.loc_.y, (Uint16)10, (Uint16)10, c.r, c.g, c.b, 255);
}

void Canvas::clear() {
  SDL_FillRect(screen_,NULL, 0x000000);
}

void Canvas::update() {
  if(screen_ != NULL) {
	  SDL_Flip(screen_);
  }
}

void Canvas::render(BattleField& field) {
	assert(field.teams_.size() == 2);
	assert(teamColors_.size() >= field.teams_.size());

	Color white = {255,255,255};
	Color red = {255,0,0};

	this->clear();
	size_t teamCnt = 0;
	for(Population& team : field.teams_) {
		for(Tank& t : team) {
			if(t.explode_)
				this->drawExplosion(t, red);
			else if(!t.dead_)
				this->drawTank(t, teamColors_[teamCnt]);
			t.explode_ = false;

			for(Projectile& p : t.projectiles_) {
				if(p.explode_)
					this->drawExplosion(p, red);
				else if(!p.dead_)
					this->drawProjectile(p,white);
				p.explode_ = false;
			}
		}
		++teamCnt;
	}

	drawText(std::to_string(field.teams_[0].score_), 50, 50, teamColors_[0]);
	drawText(std::to_string(field.teams_[1].score_), width_ - 70, 50, teamColors_[1]);

	this->update();
}
}
