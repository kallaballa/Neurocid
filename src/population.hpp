#ifndef POPULATION_HPP_
#define POPULATION_HPP_

#include <vector>
#include <iostream>
#include "ship.hpp"
#include "facility.hpp"
#include "error.hpp"

#ifndef _NO_SERIALIZE
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#endif

namespace neurocid {
using std::vector;
using std::istream;
using std::ostream;

struct PopulationLayout {
#ifndef _NO_SERIALIZE
	friend class boost::serialization::access;
#endif
	size_t size_;
	ShipLayout sl_;
	BrainLayout bl_;
	FacilityLayout fl_;

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
    ar & size_;
	  ar & sl_;
	  ar & bl_;
	  ar & fl_;
	}
#endif
};

PopulationLayout make_default_population_layout();

class Population: public vector<Ship> {
#ifndef _NO_SERIALIZE
	  friend class boost::serialization::access;
#endif

public:
	  vector<Facility> facilities_;

	struct Statistics {
		Statistics() {
			generationCnt_ = 0;
			reset();
		}

		//total fitness of population
		double totalFitness_;
		//best fitness this population
		double bestFitness_;
		//average fitness
		double averageFitness_;
		//worst
		double worstFitness_;

		double totalFriendlyFire_;
		double totalCrash_;
		double totalHits_;
		double totalDamage_;
		double totalRecharged_;
		double totalBrainSwitches_;
		double averageFriendlyFire_;
		double averageCrash_;
		double averageHits_;
		double averageDamage_;
		double averageRecharged_;
		double averageBrainSwitches_;
		size_t generationCnt_;
		double score_;

		size_t fittestGenome_;

		void reset() {
			totalFitness_ = 0;
			bestFitness_ = 0;
			worstFitness_ = std::numeric_limits<double>().max();
			averageFitness_ = 0;
			averageFriendlyFire_ = 0;
			averageCrash_ = 0;
			averageDamage_ = 0;
			averageRecharged_ = 0;
			averageBrainSwitches_ = 0;
			totalFitness_ = 0;
			totalCrash_ = 0;
			totalFriendlyFire_ = 0;
			totalHits_ = 0;
			totalDamage_ = 0;
			totalRecharged_ = 0;
			totalBrainSwitches_ = 0;
			score_ = 0;
		}

		void print(std::ostream& os) {
			os << generationCnt_ << ":"
					<< bestFitness_ << ":"
					<< averageFitness_ << ":"
					<< averageHits_ << ":"
					<< averageFriendlyFire_ << ":"
					<< averageCrash_ << ":"
					<< averageDamage_ << ":"
					<< (averageRecharged_ / 3) << ":"
					<< averageBrainSwitches_ << ":"
					<< (score_);
		}
	};

	PopulationLayout layout_;
	size_t score_ = 0;
	bool winner_ = false;
	Statistics stats_;

	void update(ShipLayout tl) {
		for (Ship& t : *this) {
			t.update(tl);
		}
	}

	size_t countDead() {
		size_t cnt = 0;
		for (Ship& t : *this) {
			if (t.dead_)
				cnt++;
		}
		return cnt;
	}

	bool isDead() {
		return countDead() == size();
	}

	Population& operator=(const Population& other) {
		layout_ = other.layout_;
		score_ = other.score_;
		winner_ = other.winner_;
		stats_ = other.stats_;
		vector<Ship>::operator=(other);
		return *this;
	}

#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	  ar & layout_;
	  ar & score_;
	  ar & winner_;
	  ar & *((vector<Ship>*)this);
	}
#endif
};

/* FIXME this is a very simple and stupid scaling mechanism.
 * scenarios should avoid scaling down a lot and scenarios
 * that scale up heavily need quite some time to adapt.
 */
inline void scale_population(Population& team, size_t size) {
	if (team.size() > size) {
		for (size_t i = size; i < team.size(); ++i) {
			team[i].brain_->destroy();
		}
		team.resize(size);
	} else if (team.size() < size) {
		while (team.size() < size) {
			for (Ship& s : team) {
				team.push_back(s.clone());
				if (team.size() == size)
					break;
			}
		}
	}
}

inline void read_population(size_t teamID, Population& team, istream& is) {
#ifndef _NO_SERIALIZE
  boost::archive::binary_iarchive ia(is);
  ia >> team;

  for(Ship& s : team) {
	  s.teamID_ = teamID;
  }
  scale_population(team, team.layout_.size_);
#else
  CHECK(false);
#endif
}

inline void write_population(Population& team, ostream& os) {
#ifndef _NO_SERIALIZE
  boost::archive::binary_oarchive oa(os);
  scale_population(team, team.layout_.size_);
  oa << team;
#else
  CHECK(false);
#endif
}
}

#endif /* POPULATION_HPP_ */
