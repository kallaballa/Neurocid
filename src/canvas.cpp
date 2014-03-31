
#include "canvas.hpp"
#include "tank.hpp"
#include "projectile.hpp"
#include "battlefield.hpp"
#include "population.hpp"
#include "time_tracker.hpp"
#include <sstream>
#include <thread>
#include <chrono>
#include <iostream>


namespace tankwar {
using std::stringstream;
using std::cerr;
using std::endl;

Canvas* Canvas::instance_ = NULL;

Canvas::Canvas(Coord width, Coord height) :
		screen_(NULL),
		drawEngines_(false),
		drawCenters_(false),
		drawGrid_(false),
		width_(width),
		height_(height),
		scale_(1),
		viewPort_() {
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
	if (TTF_Init() != 0) {
		cerr << "TTF_Init() Failed: " << TTF_GetError() << endl;
		SDL_Quit();
		exit(1);
	}

	// Load a font
	font_ = TTF_OpenFont("/usr/share/fonts/truetype/DejaVuSansMono.ttf", 12);
	if (font_ == NULL) {
		cerr << "TTF_OpenFont() Failed: " << TTF_GetError() << endl;
		TTF_Quit();
		SDL_Quit();
		exit(1);
	}
}

void Canvas::calculateScale() {
	Coord scaleX = width_ / (viewPort_.lr_.x_ - viewPort_.ul_.x_);
	Coord scaleY = height_ / (viewPort_.lr_.y_ - viewPort_.ul_.y_);
	scale_ = std::min(scaleX, scaleY);
}

Sint16 Canvas::scaleX(const Coord& c) {
	Coord preScale = 0.9;
	Coord len = (viewPort_.lr_.x_ - viewPort_.ul_.x_) * scale_ * preScale;
	Coord pos = (c - viewPort_.ul_.x_) * scale_ * preScale;
	Coord scaled = (((width_ - len) / 2) + pos);

	return (Sint16)round(scaled);
}

Sint16 Canvas::scaleY(const Coord& c) {
	Coord preScale = 0.9;
	Coord len = (viewPort_.lr_.y_ - viewPort_.ul_.y_) * scale_ * preScale;
	Coord pos = (c - viewPort_.ul_.y_) * scale_ * preScale;
	Coord scaled = (((height_ - len) / 2) + pos);

	return (Sint16)round(scaled);
}

void Canvas::drawEllipse(Vector2D loc, Coord rangeX, Coord rangeY, Color c) {
	ellipseRGBA(screen_, scaleX(loc.x_), scaleY(loc.y_), rangeX * scale_, rangeY * scale_ , c.r, c.g, c.b, 255);
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

void Canvas::updateOSD(const string& key, const string& value) {
	osdMap_[key] = value;
}

void Canvas::renderOSD() {
	Coord y = 20;
	for(auto it : osdMap_) {
		SDL_Surface *text;
		SDL_Color text_color = { 255, 255, 255 };
		text = TTF_RenderText_Solid(font_, (it.first + ": " + it.second).c_str(), text_color);
		assert(text != NULL);
		SDL_Rect drest = {20,(Sint16)round(y),0,0};
		y += (text->h + 5);

		if (SDL_BlitSurface(text, NULL, screen_, &drest) != 0) {
			cerr << "SDL_BlitSurface() Failed: " << SDL_GetError() << endl;
		}
		SDL_FreeSurface(text);
	}
}

Coord Canvas::renderText(const string& s, Coord y, Color c, bool left) {
	SDL_Surface *text;
	SDL_Color text_color = { c.r, c.g, c.b };
	text = TTF_RenderText_Solid(font_, s.c_str(), text_color);
	assert(text != NULL);

	SDL_Rect drest;
	if(!left)
		drest = {(Sint16)round(width_ - text->w), (Sint16)round(y),0,0};
	else
		drest = {20, (Sint16)round(y),0,0};

	if (SDL_BlitSurface(text, NULL, screen_, &drest) != 0) {
		cerr << "SDL_BlitSurface() Failed: " << SDL_GetError() << endl;
	}
	Coord h = text->h;
	SDL_FreeSurface(text);
	return h;
}

void Canvas::renderTackerInfo() {
	Coord y = 20;
	const map<string, TimeInfo>& tiMap = TimeTracker::getInstance()->getMap();
	Color c = {200,200,200};
	stringstream ss;
	for(auto it : tiMap) {
		ss << std::left << std::setw(45) << std::setfill(' ') << (it.first + ": " + it.second.str());
		y += renderText(ss.str(), y, c, false) + 5;
		ss.str("");

		for(auto itc : it.second.children_) {
			ss << std::left << std::setw(45) << std::setfill(' ') << ("  "  + itc.first + ": " + itc.second.str());
			y += renderText(ss.str(), y, c, false) + 5;
			ss.str("");
		}
	}
}

void fill_circle(SDL_Surface *surface, int cx, int cy, int radius, Uint32 pixel)
{
    // Note that there is more to altering the bitrate of this
    // method than just changing this value.  See how pixels are
    // altered at the following web page for tips:
    //   http://www.libsdl.org/intro.en/usingvideo.html
    static const int BPP = 4;

    double r = (double)radius;

    for (double dy = 1; dy <= r; dy += 1.0)
    {
        // This loop is unrolled a bit, only iterating through half of the
        // height of the circle.  The result is used to draw a scan line and
        // its mirror image below it.

        // The following formula has been simplified from our original.  We
        // are using half of the width of the circle because we are provided
        // with a center and we need left/right coordinates.

        double dx = floor(sqrt((2.0 * r * dy) - (dy * dy)));
        int x = cx - dx;

        // Grab a pointer to the left-most pixel for each half of the circle
        Uint8 *target_pixel_a = (Uint8 *)surface->pixels + ((int)(cy + r - dy)) * surface->pitch + x * BPP;
        Uint8 *target_pixel_b = (Uint8 *)surface->pixels + ((int)(cy - r + dy)) * surface->pitch + x * BPP;

        for (; x <= cx + dx; x++)
        {
            *(Uint32 *)target_pixel_a = pixel;
            *(Uint32 *)target_pixel_b = pixel;
            target_pixel_a += BPP;
            target_pixel_b += BPP;
        }
    }
}

void Canvas::drawTank(Tank& tank, Color c) {
	Vector2D dir = tank.getDirection();
	Vector2D tip = tank.loc_;
    tip += dir * (tank.range_) * 5;

    drawLine(tank.loc_.x_, tank.loc_.y_, tip.x_, tip.y_ ,c);
    drawEllipse(tank.loc_, tank.range_, tank.range_, c);
	drawLine(tank.loc_.x_, tank.loc_.y_, tip.x_, tip.y_ ,c);
	if(drawEngines_) {
		Vector2D across1 = dir;
		Vector2D across2 = dir;
		across2.rotate(90);

		Vector2D wc = tank.loc_;
		Vector2D flengine = wc;
		Vector2D frengine = wc;
		Vector2D blengine = wc;
		Vector2D brengine = wc;

		flengine += (across1 * (tank.range_));
		frengine += (across2 * (tank.range_));
		blengine += (across2 * -(tank.range_));
		brengine += (across1 * -(tank.range_));

		Vector2D flforce = flengine;
		flforce += across1 * -(tank.flthrust_ * 600);
		Vector2D frforce = frengine;
		frforce += across2 * (tank.frthrust_ * 600);
		Vector2D blforce = blengine;
		blforce += across1 * (tank.blthrust_ * 600);
		Vector2D brforce = brengine;
		brforce += across2 * -(tank.brthrust_ * 600);
		Color red = {255,0,0};
		drawLine(flengine.x_, flengine.y_, flforce.x_, flforce.y_, red);
		drawLine(frengine.x_, frengine.y_, frforce.x_, frforce.y_, red);
		drawLine(blengine.x_, blengine.y_, blforce.x_, blforce.y_, red);
		drawLine(brengine.x_, brengine.y_, brforce.x_, brforce.y_, red);
	}
}

void Canvas::drawProjectile(Projectile& pro, Color& c) {
	Vector2D trail = pro.loc_;
	trail += pro.getDirection() * -50;
	drawLine(trail.x_,trail.y_, pro.loc_.x_, pro.loc_.y_, c);
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

void Canvas::drawGrid(BattleField& field) {
	Color grey = {32,32,32};
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

void Canvas::drawCenters(Scanner& scanner) {
	for(Vector2D center : scanner.centersA_) {
		drawEllipse(center, 3/ scale_, 3/ scale_, teamColors_[0]);
		drawEllipse(center, 10/ scale_, 10/ scale_, teamColors_[0]);
	}

	for(Vector2D center : scanner.centersB_) {
		drawEllipse(center, 3/ scale_, 3/ scale_, teamColors_[1]);
		drawEllipse(center, 10/ scale_, 10/ scale_, teamColors_[1]);
	}
}

void Canvas::render(BattleField& field) {
	assert(field.teams_.size() == 2);
	assert(teamColors_.size() >= field.teams_.size());
	viewPort_ = findBounds(field);
	calculateScale();
	this->clear();

	if(drawGrid_)
		drawGrid(field);

	Color red = {255,0,0};
	Color neonYellow = {243,243,21};
	Color neonOrange = {255, 100, 100};

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
				else if(!p->dead_) {
					if(t.teamID_ == 0)
						this->drawProjectile(*p,neonYellow);
					else
						this->drawProjectile(*p,neonOrange);
				}
				p->explode_ = false;
			}
		}
		++teamCnt;
	}

	updateOSD("Score", std::to_string(field.teams_[0].score_) + " : " + std::to_string(field.teams_[1].score_));
	updateOSD("Best", std::to_string(field.teams_[0].stats_.bestFitness_) + " / " + std::to_string(field.teams_[1].stats_.bestFitness_));
	updateOSD("Avg", std::to_string(field.teams_[0].stats_.averageFitness_) + " / " + std::to_string(field.teams_[1].stats_.averageFitness_));
	updateOSD("Gen", std::to_string(field.teams_[0].stats_.generationCnt_) + " / " + std::to_string(field.teams_[1].stats_.generationCnt_));

	renderOSD();
	if(TimeTracker::getInstance()->isEnabled())
		renderTackerInfo();

	if(drawCenters_)
		drawCenters(field.scanner_);
	this->update();
}
}
