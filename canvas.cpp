
#include "canvas.hpp"
#include "tank.hpp"
#include "projectile.hpp"

namespace tankwar {

Canvas::Canvas(Coord width, Coord height) :
  screen_(NULL) , width_(width), height_(height) {
  if (width > 0 && height > 0) {
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
      printf("Can't init SDL:  %s\n", SDL_GetError());
      exit(1);
    }
    atexit(SDL_Quit);
    screen_ = SDL_SetVideoMode(width, height, 16, SDL_SWSURFACE);
    if (screen_ == NULL) {
      printf("Can't set video mode: %s\n", SDL_GetError());
      exit(1);
    }
  }
}

void Canvas::drawLine(Coord x0, Coord y0, Coord x1, Coord y1, Color& c) {
  if(screen_ != NULL) {
    lineRGBA(screen_, x0, y0, x1, y1, c.r, c.g, c.b, 128);
  }
}

void Canvas::drawTank(Tank& tank, Color& c) {
    ellipseRGBA(screen_, (Uint16)tank.loc_.x, (Uint16)tank.loc_.y, (Uint16)tank.range_, (Uint16)tank.range_, c.r, c.g, c.b, 255);
    Vector2D tip = tank.loc_;
    tip += tank.dir_ * 5;
    ellipseRGBA(screen_, (Uint16)tip.x, (Uint16)tip.y, 3, 3, c.r, c.g, c.b, 255);
}

void Canvas::drawProjectile(Projectile& pro, Color& c) {
    ellipseRGBA(screen_, (Uint16)pro.loc_.x, (Uint16)pro.loc_.y, (Uint16)pro.range_, (Uint16)pro.range_, c.r, c.g, c.b, 255);
}

void Canvas::drawExplosion(Object& o, Color& c) {
	 ellipseRGBA(screen_, (Uint16)o.loc_.x, (Uint16)o.loc_.y, (Uint16)o.range_, (Uint16)o.range_, c.r, c.g, c.b, 255);
}

void Canvas::clear() {
  SDL_FillRect(screen_,NULL, 0x000000);
}

void Canvas::update() {
  if(screen_ != NULL) {
	  SDL_Flip(screen_);
  }
}
}
