/*
 * placer.hpp
 *
 *  Created on: Mar 7, 2014
 *      Author: elchaschab
 */

#ifndef PLACER_HPP_
#define PLACER_HPP_

#include <vector>
#include "util.hpp"
#include "2d.hpp"
#include "population.hpp"
#include <functional>
#include <iostream>

namespace tankwar {
using std::vector;
using std::function;

class RandomRot {
public:
	Coord operator()(size_t tick) {
		return fRand(M_PI, -M_PI);
	}
};

class IterRot {
	double startInRad_;
	double iterateByInRad_;
public:
	IterRot(double startInRad, double iterateByInRad) :
		startInRad_(startInRad),
		iterateByInRad_(iterateByInRad){

	};

	Coord operator()(size_t tick) {
		return normRotation(startInRad_ + (iterateByInRad_ * ((tick % ((size_t)(round(M_PI/iterateByInRad_) + 1))))));
	}
};

class RandomFacer {
public:
	Coord operator()(size_t tick, size_t side, Coord rotation) {
		return fRand(-M_PI, M_PI);
	}
};

class OppositeFacer {
	Coord relativeRotation_;
public:
	OppositeFacer(Coord relativeRotation) : relativeRotation_(relativeRotation) {
	}
	Coord operator()(size_t tick, size_t side, Coord rotation) {
		assert(side < 2);
		return normRotation(radFromDir(dirFromRad(rotation) * (side == 0 ? 1 : -1)) + (relativeRotation_));
	}
};

struct GameLayout {
	Vector2D center_;
	Coord distance_;
	Coord spacing_;
};

class Layouter {
	GameLayout layout_;
public:
	Layouter(const GameLayout& layout) :
	 layout_(layout){
	}
	GameLayout operator()(size_t tick, size_t side) {
		return {layout_.center_, layout_.distance_, layout_.spacing_};
	}
};

class Placer {
	size_t tick_ = 0;
public:
	size_t tick() {
		return tick_;
	}

	virtual void place(vector<Population>& teams) {
		tick_++;
	}
};

template <typename Trotator, typename Tfacer, typename Tlayouter> class OppositePlacer : public Placer {
	Trotator rotator_;
	Tfacer facer_;
	Tlayouter layouter_;
public:
	OppositePlacer(Trotator rotator, Tfacer facer, Tlayouter layouter) :
		rotator_(rotator),
		facer_(facer),
		layouter_(layouter) {
	}

	virtual void place(vector<Population>& teams) {
		assert(teams.size() == 2);
		double rotation = rotator_(tick());
		GameLayout gl = layouter_(tick(), 0);
		Vector2D axisDir = dirFromRad(rotation);

		Vector2D sideDirA = axisDir;
		Vector2D sideDirB = axisDir;

		sideDirA.rotate(90);
		sideDirB = sideDirA * -1;

		Vector2D centerA = gl.center_;
		Vector2D centerB = gl.center_;
		centerA += (axisDir * (gl.distance_/2));
		centerB -= (axisDir * (gl.distance_/2));

		Coord lengthA = (((teams[0][0].range_ + gl.spacing_) * (teams[0].size() - 1)) / 2);
		Coord lengthB = (((teams[1][0].range_ + gl.spacing_) * (teams[1].size() - 1)) / 2);

		Vector2D startA = centerA;
		startA += (sideDirA * lengthA);
		Vector2D startB = centerB;
		startB += (sideDirB * lengthB);

		for(size_t i = 0; i < teams[0].size(); i++) {
			teams[0][i].loc_ = startA;
			teams[0][i].loc_ -= (sideDirA * ((teams[0][i].range_ + gl.spacing_) * i));
			teams[0][i].rotation_ = facer_(tick(), 0, rotation);
		}

		for(size_t i = 0; i < teams[1].size(); i++) {
			teams[1][i].loc_ = startB;
			teams[1][i].loc_ -= (sideDirB * ((teams[1][i].range_ + gl.spacing_) * i));
			teams[1][i].rotation_ = facer_(tick(), 1, rotation);
		}
		Placer::place(teams);
	}
};


} /* namespace tankwar */

#endif /* PLACER_HPP_ */
