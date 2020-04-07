#include "felgocanvas.hpp"

#include "facility.hpp"
#include "battlefield.hpp"
#include "population.hpp"
#include "time_tracker.hpp"
#include "../theme.hpp"

#include <sstream>
#include <chrono>
#include <iostream>
#include <algorithm>

namespace neurocid {
using std::stringstream;
using std::cerr;
using std::endl;

FelgoCanvas::FelgoCanvas(FelgoGFX* felgoGfx, Coord width, Coord height, BattleFieldLayout& bfl) :
		Canvas(), felgoGfx_(felgoGfx), width_(width), height_(height), scale_(1), zoom_(
				1), viewPort_(), background_(this), bfl_(bfl) {

	background_.init(bfl);
	viewPort_.ul_ = {320000,320000};
	viewPort_.lr_ = {330000,330000};
}

FelgoCanvas::~FelgoCanvas() {
}

void FelgoCanvas::calculateScale() {
	Coord preScale = 0.9;
	Coord scaleX = width_ / (viewPort_.lr_.x_ - viewPort_.ul_.x_);
	Coord scaleY = height_ / (viewPort_.lr_.y_ - viewPort_.ul_.y_);

	scale_ = std::min(scaleX, scaleY) * preScale / zoom_;
}

std::pair<int16_t, int16_t> FelgoCanvas::scale(const Vector2D& v, Coord z) {
	Coord px = v.x_ - viewPort_.lr_.x_;
	Coord py = v.y_ - viewPort_.lr_.y_;
	Coord pz = z * 100000;
	Coord focus = 10000;

	Coord x = (px * focus) / (pz + 1000000);
	Coord y = (py * focus) / (pz + 1000000);

	x *= scale_;
	x += width_ / 2;
	y *= scale_;
	y += height_ / 2;
    return {static_cast<int16_t>(round(x)), static_cast<int16_t>(round(y))};
}

std::pair<int16_t, int16_t> FelgoCanvas::transform(const Vector2D& v, Coord z) {
	Coord cx = v.x_ - viewPort_.lr_.x_;
	Coord cy = v.y_ - viewPort_.lr_.y_;

	z = z * 100000;
	Coord px = cx;
	Coord py = cos(angle_) * cy + sin(angle_) * z;
	Coord pz = -sin(angle_) * cy + cos(angle_) * z;

	Coord focus = 10000;

	Coord x = (px * focus) / (pz + 1000000);
	Coord y = (py * focus) / (pz + 1000000);

	x *= scale_;
	x += width_ / 2;
	y *= scale_;
	y += height_ / 2;

    return {static_cast<int16_t>(round(x)), static_cast<int16_t>(round(y))};
}

void FelgoCanvas::zoomIn() {
	zoom_ *= 0.9;
	zoom_ = std::max(zoom_, 0.001);
}

void FelgoCanvas::zoomOut() {
	zoom_ *= 1.1;
}

void FelgoCanvas::left() {
	viewPort_.ul_.x_ -= (150000 * zoom_);
	viewPort_.lr_.x_ -= (150000 * zoom_);
}

void FelgoCanvas::right() {
	viewPort_.ul_.x_ += (150000 * zoom_);
	viewPort_.lr_.x_ += (150000 * zoom_);
}

void FelgoCanvas::up() {
	viewPort_.ul_.y_ -= (150000 * zoom_);
	viewPort_.lr_.y_ -= (150000 * zoom_);
}

void FelgoCanvas::down() {
	viewPort_.ul_.y_ += (150000 * zoom_);
	viewPort_.lr_.y_ += (150000 * zoom_);
}

void FelgoCanvas::tiltUp() {
	angle_ *= 1.1;
	if (angle_ > M_PI)
		angle_ = M_PI;
}

void FelgoCanvas::tiltDown() {
	angle_ *= 0.9;
	if (angle_ < 0.1)
		angle_ = 0.1;
}

void FelgoCanvas::drawStar(Star& s) {
	double alpha = s.alpha;
	for (size_t i = 0; i < s.radius; i++) {
        felgoGfx_->circleRGBA(int16_t(s.x), int16_t(s.y), int16_t(i), s.r, s.g, s.b, uint8_t(round(alpha)));

		if (i == s.discontinuity)
			alpha /= 3;
		alpha *= s.step;
	}
}

void FelgoCanvas::drawExplosion(Explosion& expl) {
#ifndef _NO_SDLGFX
	size_t lastR = 0, r = 0;
	for (size_t i = expl.tick_; i > 0; --i) {
		if (scale_ < 0.013)
            r = size_t(round((pow(i, 2) / 30) * 6 * scale_));
		else
            r = size_t(round((pow(i, 2) / 30) * scale_));

		Color c = expl.color_ + Color(0, 10, 0);
        c.a = int(std::min(size_t(200 + (255 / (i + 1))), size_t(255)));
		if (lastR != r) {
			auto scaled = transform(expl.loc_);
            felgoGfx_->circleRGBA(scaled.first, scaled.second, int16_t(round(r * scale_)), uint8_t(c.r), uint8_t(c.g), uint8_t(c.b), uint8_t(c.a));
		}
		lastR = r;
	}
#endif
}

void FelgoCanvas::drawTrail(const Trail& trail, const Color& c) {
#ifndef _NO_SDLGFX
	size_t cnt = 0;
    int16_t lastX = -1;
    int16_t lastY = -1;

	for (const Vector2D& v : trail) {
		auto scaled = transform(v);
		size_t r;
		if (scale_ < 0.013)
            r = size_t(round(1.5 / (cnt + 1) * scale_));
		else
            r = size_t(round(0.5 / (cnt + 1) * scale_));

        felgoGfx_->filledCircleRGBA(scaled.first, scaled.second, int16_t(r), uint8_t(c.r), uint8_t(c.g), uint8_t(c.b), uint8_t(c.a));

		if (lastX != -1 && lastY != -1) {
            felgoGfx_->lineRGBA(lastX, lastY, scaled.first, scaled.second, uint8_t(c.r), uint8_t(c.g), uint8_t(c.b), 48);
		}

		lastX = scaled.first;
		lastY = scaled.second;

		++cnt;
	}
#endif
}

void FelgoCanvas::drawEllipse(Vector2D loc, Coord rangeX, Coord rangeY, Color c) {
	auto scaled = transform(loc);
    felgoGfx_->ellipseRGBA(scaled.first, scaled.second, int16_t(round(rangeX * scale_)), int16_t(round(rangeY * scale_)), uint8_t(c.r), uint8_t(c.g), uint8_t(c.b), uint8_t(c.a));
}

void FelgoCanvas::fillCircle(Vector2D loc, Coord radius, Color c) {
#ifndef _NO_SDLGFX
	auto scaled = transform(loc);
    felgoGfx_->filledCircleRGBA(scaled.first, scaled.second, int16_t(round(radius * scale_)), uint8_t(c.r), uint8_t(c.g), uint8_t(c.b), uint8_t(c.a));
#endif
}

void FelgoCanvas::drawLine(Coord x0, Coord y0, Coord x1, Coord y1, Color c,
		Coord s) {
	auto scaled0 = transform(Vector2D(x0, y0), s);
	auto scaled1 = transform(Vector2D(x1, y1), s);
    felgoGfx_->lineRGBA(scaled0.first, scaled0.second, scaled1.first, scaled1.second, uint8_t(c.r), uint8_t(c.g), uint8_t(c.b), uint8_t(c.a));
}

void FelgoCanvas::drawRect(Coord x0, Coord y0, Coord x1, Coord y1, Color c,
		Coord s) {
	auto scaled0 = transform(Vector2D(x0, y0), s);
	auto scaled1 = transform(Vector2D(x1, y1), s);
    felgoGfx_->rectangleRGBA(scaled0.first, scaled0.second, scaled1.first, scaled1.second, uint8_t(c.r), uint8_t(c.g), uint8_t(c.b), uint8_t(c.a));
}

void FelgoCanvas::fillRect(Coord x0, Coord y0, Coord x1, Coord y1, Color c,
		Coord s) {
	auto scaled0 = transform(Vector2D(x0, y0), s);
	auto scaled1 = transform(Vector2D(x1, y1), s);
    const int16_t xv[5] = { scaled0.first, scaled1.first, scaled1.first,
			scaled0.first, scaled0.first };
    const int16_t yv[5] = { scaled0.second, scaled0.second, scaled1.second,
			scaled1.second, scaled0.second };

    felgoGfx_->filledPolygonRGBA(xv, yv, 5, uint8_t(c.r), uint8_t(c.g), uint8_t(c.b), uint8_t(c.a));
}
void FelgoCanvas::drawShip(Ship& ship, Color c) {
	Vector2D dir = ship.getDirection();
	Vector2D tip = ship.loc_;
	if (scale_ < 0.013)
		tip += dir * (ship.radius_) * 10;
	else
		tip += dir * (ship.radius_) * 5;

	drawLine(ship.loc_.x_, ship.loc_.y_, tip.x_, tip.y_, c);
	if (scale_ < 0.013)
		drawEllipse(ship.loc_, ship.radius_ / 30, ship.radius_ / 30, c);
	else
		drawEllipse(ship.loc_, ship.radius_ / 100, ship.radius_ / 100, c);

	if (ship.stun_ > 0 && !ship.layout_.isDummy_) {
		if (scale_ < 0.013)
			drawEllipse(ship.loc_, ship.radius_, ship.radius_,
					{ 255, 255, 255 });
		else
			drawEllipse(ship.loc_, ship.radius_, ship.radius_,
					{ 255, 255, 255 });
	}
	drawLine(ship.loc_.x_, ship.loc_.y_, tip.x_, tip.y_, c);

	if (isDrawEliteEnabled() && ship.isElite) {
		drawEllipse(ship.loc_, 2, 2, { 255, 0, 255 });
	}

	Color cengine;
	if (ship.teamID_ == 0)
		cengine = Theme::enginesA;
	else
		cengine = Theme::enginesB;

	if (isDrawEnginesEnabled()) {
		Vector2D across1 = dir;
		Vector2D across2 = dir;
		across1.rotate(-45);
		across2.rotate(45);

		Vector2D wc = ship.loc_;
		Vector2D flengine = wc;
		Vector2D frengine = wc;
		Vector2D blengine = wc;
		Vector2D brengine = wc;

		flengine += (dir * (ship.radius_ / 2));
		frengine += (dir * (ship.radius_ / 2));
		blengine += (dir * -(ship.radius_ / 2));
		brengine += (dir * -(ship.radius_ / 2));

		Vector2D flforce = flengine;
		flforce += across2 * -(ship.flthrust_ * 6000);
		Vector2D frforce = frengine;
		frforce += across1 * (ship.frthrust_ * 6000);
		Vector2D blforce = blengine;
		blforce += across1 * (ship.blthrust_ * 6000);
		Vector2D brforce = brengine;
		brforce += across2 * -(ship.brthrust_ * 6000);

		drawLine(flengine.x_, flengine.y_, flforce.x_, flforce.y_, cengine);
		drawLine(frengine.x_, frengine.y_, frforce.x_, frforce.y_, cengine);
		drawLine(blengine.x_, blengine.y_, blforce.x_, blforce.y_, cengine);
		drawLine(brengine.x_, brengine.y_, brforce.x_, brforce.y_, cengine);
	}

	if (ship.isJumping_) {
		Vector2D center = ship.loc_;
		Vector2D across1 = dir;
		Vector2D across2 = dir;
		across1.rotate(-45);
		across2.rotate(45);
		Vector2D flforce;
		flforce += across2 * -(ship.flthrust_ * ship.layout_.maxSpeed_ * 240);
		Vector2D frforce;
		frforce += across1 * (ship.frthrust_ * ship.layout_.maxSpeed_ * 240);
		Vector2D blforce;
		blforce += across1 * (ship.blthrust_ * ship.layout_.maxSpeed_ * 240);
		Vector2D brforce;
		brforce += across2 * -(ship.brthrust_ * ship.layout_.maxSpeed_ * 240);

		Vector2D totalForce;
		totalForce += flforce;
		totalForce += frforce;
		totalForce += blforce;
		totalForce += brforce;

		Color jumpColor = c * 2;
		drawLine(center.x_, center.y_, center.x_ + totalForce.x_ * -30,
				center.y_ + totalForce.y_ * -30, jumpColor);
	}
}

void FelgoCanvas::drawFacility(Facility& facility, Color c) {
#ifndef _NO_SDLGFX
	if (facility.recharged_ >= facility.layout_.maxRecharge_)
		c = c * 0.5;

	auto scaled = transform(facility.loc_);
    felgoGfx_->filledCircleRGBA(scaled.first, scaled.second, int16_t(round((facility.layout_.radius_ / 100) * scale_)), uint8_t(c.r), uint8_t(c.g), uint8_t(c.b), 128);

	for (size_t i = 0; i < 50; ++i) {
		drawEllipse(facility.loc_, facility.layout_.radius_ / 100 - i,
				facility.layout_.radius_ / 100 - i, c);
	}

	for (size_t i = 0; i < 3; ++i) {
        int16_t start = int16_t(iRand(0, 360));
        int16_t end = int16_t(iRand(start, 360));
        felgoGfx_->pieRGBA(scaled.first, scaled.second, int16_t(round(scale_ * (facility.layout_.radius_ / 150))), start, end, 0, 0, 255, 255);
        start = int16_t(iRand(0, 360));
        end = int16_t(iRand(start, 360));
        felgoGfx_->pieRGBA(scaled.first, scaled.second, int16_t(round(scale_ * (facility.layout_.radius_ / 150))), start, end, 255, 255, 0, 255);
	}

	Vector2D ul(facility.loc_.x_ + facility.layout_.radius_ + 1000,
			facility.loc_.y_ - facility.layout_.radius_ / 2 - 4000);
	Vector2D lr(ul.x_ + 1500, ul.y_ + 15000);
	Coord height = lr.y_ - ul.y_;
	height *=
			(1.0
                    - (static_cast<double>(facility.damage_)
                            / static_cast<double>(facility.layout_.maxDamage_)));
	ul.y_ = lr.y_ - height;
	fillRect(ul.x_, ul.y_, lr.x_, lr.y_, { 255, 255, 255 });
#endif
}

void FelgoCanvas::drawProjectile(Projectile& pro, Color& c) {
	if (!isDrawProjectilesEnabled())
		return;

	Vector2D trail = pro.loc_;
	if (scale_ < 0.013)
		trail += pro.getDirection() * -1500;
	else
		trail += pro.getDirection() * -500;

	drawLine(trail.x_, trail.y_, pro.loc_.x_, pro.loc_.y_, c);
}

void FelgoCanvas::clear() {
    felgoGfx_->clear(0,0,0);
	background_.draw();
}

void FelgoCanvas::update() {
	felgoGfx_->flip();
}

Rect FelgoCanvas::findBounds(BattleField& field) {
	Vector2D min(std::numeric_limits<Coord>().max(),
			std::numeric_limits<Coord>().max());
	Vector2D max(0, 0);

	for (Population& team : field.teams_) {
		for (Ship& t : team) {
			if (t.dead_)
				continue;
			min.x_ = std::min(t.loc_.x_, min.x_);
			min.y_ = std::min(t.loc_.y_, min.y_);
			max.x_ = std::max(t.loc_.x_, max.x_);
			max.y_ = std::max(t.loc_.y_, max.y_);
		}
	}

	for (Explosion& e : explosions_) {
		min.x_ = std::min(e.loc_.x_, min.x_);
		min.y_ = std::min(e.loc_.y_, min.y_);
		max.x_ = std::max(e.loc_.x_, max.x_);
		max.y_ = std::max(e.loc_.y_, max.y_);
	}

	auto minT = scale(min, 1);
	auto maxT = scale(max, 1);
	min.x_ = minT.first;
	min.y_ = minT.second;
	max.x_ = maxT.first;
	max.y_ = maxT.second;

	return {min, max};
}

void FelgoCanvas::drawBorder(BattleField& field) {
	for (Coord z = 0.9; z <= 1.1; z += 0.05) {
		drawLine(0, 0, 0, field.layout_.height_, Theme::battleFieldBorder, z);
		drawLine(0, 0, field.layout_.width_, 0, Theme::battleFieldBorder, z);
		drawLine(field.layout_.width_, field.layout_.height_,
				field.layout_.width_, 0, Theme::battleFieldBorder, z);
		drawLine(field.layout_.width_, field.layout_.height_, 0,
				field.layout_.height_, Theme::battleFieldBorder, z);
	}
}

void FelgoCanvas::drawGrid(BattleField& field) {
	for (Coord x = 0; x < field.layout_.width_; x += 15000) {
		drawLine(x, 0, x, field.layout_.height_, Theme::battleFieldGrid);
	}
	for (Coord y = 0; y < field.layout_.height_; y += 15000) {
		drawLine(0, y, field.layout_.width_, y, Theme::battleFieldGrid);
	}

	drawEllipse(Vector2D(field.layout_.width_ / 2, field.layout_.height_ / 2),
			20, 20, Theme::battleFieldCenter);
}

void FelgoCanvas::drawCenters(Scanner& scanner) {
	for (Vector2D center : scanner.centersA_) {
		drawEllipse(center, 3 / scale_, 3 / scale_, Theme::teamA);
		drawEllipse(center, 10 / scale_, 10 / scale_, Theme::teamA);
	}

	for (Vector2D center : scanner.centersB_) {
		drawEllipse(center, 3 / scale_, 3 / scale_, Theme::teamB);
		drawEllipse(center, 10 / scale_, 10 / scale_, Theme::teamB);
	}
}

void FelgoCanvas::pushTrail(Ship& t) {
	if (trails_.find(&t) == trails_.end())
		trails_[&t] = Trail();

	Trail& trail = trails_[&t];
	trail.push_front(t.loc_);
	if (t.teamID_ == 0)
		trail.color_ = Theme::teamA;
	else
		trail.color_ = Theme::teamB;

	if (trail.size() > 5)
		trail.pop_back();
}

void FelgoCanvas::render(BattleField& field) {
//	if(zoom_ == 1)
//		viewPort_ = findBounds(field);
	calculateScale();

	if (isDrawGridEnabled())
		drawGrid(field);

	drawBorder(field);

	size_t teamCnt = 0;
	for (Population& team : field.teams_) {
		for (Facility& f : team.facilities_) {
			if (f.dead_)
				continue;
			if (f.teamID_ == 0)
				this->drawFacility(f, Theme::teamA);
			else
				this->drawFacility(f, Theme::teamB);
		}

		for (Ship& t : team) {
			if (t.explode_)
				explosions_.push_back( { t.loc_, 0, 20, { 255, 64, 0, 255 } });

			if (t.crashed_)
				explosions_.push_back(
						{ t.loc_, 0, 20, { 64, 128, 255, 128 } });
			else if (!t.dead_) {
				auto it = trails_.find(&t);
				if (!t.dead_ && it != trails_.end())
					drawTrail((*it).second, (*it).second.color_);

				if (t.teamID_ == 0)
					this->drawShip(t, Theme::teamA);
				else
					this->drawShip(t, Theme::teamB);
			}

			t.explode_ = false;
			t.crashed_ = false;
			for (Projectile* p : t.projectiles_) {
				//				if(p->explode_)
//					explosions_.push_back({p->loc_, 0, 20, {255,64,0,255}});

                if (p->blast_ != nullptr && !p->blast_->dead_)
					explosions_.push_back( { p->loc_, 35, 40,
							{ 255, 128, 0, 255 } });

				else if (!p->dead_) {
					if (t.teamID_ == 0)
						this->drawProjectile(*p, Theme::projectileA);
					else
						this->drawProjectile(*p, Theme::projectileB);
				}
				p->explode_ = false;
			}

			pushTrail(t);
		}
		++teamCnt;
	}

	for (auto it = explosions_.begin(); it != explosions_.end(); ++it) {
		Explosion& e = *it;
		if (e.end())
			it = explosions_.erase(it);
		else {
			drawExplosion(e);
			e.next();
		}
	}

	if (isDrawCentersEnabled())
		drawCenters(field.scanner_);
}

void FelgoCanvas::reset() {
	explosions_.clear();
}
}
