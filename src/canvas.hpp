#ifndef CANVAS_H_
#define CANVAS_H_

#include "2d.hpp"
#include "SDL/SDL.h"

namespace neurocid {
using std::string;

class BattleField;

class Canvas {
private:
	static Canvas* instance_;
	bool drawCenters_ = false;
	bool drawEngines_ = false;
	bool drawGrid_ = false;
	bool drawProjectiles_ = true;
	bool drawElite_ = false;
public:
	Canvas() {};
	virtual ~Canvas() {};

	virtual void zoomIn() = 0;
	virtual void zoomOut() = 0;
	virtual void left() = 0;
	virtual void right() = 0;
	virtual void up() = 0;
	virtual void down() = 0;

	virtual void update() = 0;
	virtual void clear() = 0;
	virtual void render(BattleField& field) = 0;
	virtual void reset() = 0;

	virtual Coord width() const = 0;
	virtual Coord height() const = 0;
	virtual SDL_Surface* getSurface() const = 0;

	static void init(Canvas* canvas) {
		assert(instance_ == NULL);
		instance_ = canvas;
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
