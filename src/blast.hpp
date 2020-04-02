#ifndef BLAST_HPP_
#define BLAST_HPP_

#include "object.hpp"
#include "scanner.hpp"
#include <limits>

#ifndef _NO_SERIALIZE
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#endif

namespace neurocid {

class Projectile;

struct BlastLayout {
#ifndef _NO_SERIALIZE
	friend class boost::serialization::access;
#endif
	Coord radius_;
  Coord maxColateral_;


#ifndef _NO_SERIALIZE
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
    //suppress warning about unused variable version
    assert(version >= 0);
	  ar & radius_;
	  ar & maxColateral_;
	}
#endif
};

class Blast : public Object {
public:
	Projectile* owner_;
	BlastLayout layout_;

  Blast(Projectile& owner, BlastLayout& layout, Vector2D& loc) :
      Object(BLAST, loc, 0, layout.radius_, false, false, false), owner_(&owner), layout_(layout) {
  }

	void move(BattleFieldLayout& bfl) {
	}

	void death() {
		dead_ = true;
		explode_ = true;
	}
};
}

#endif /* BLAST_HPP_ */
