#ifndef BSB_H_
#define BSB_H_

#include "error.hpp"
#include "object.hpp"
#include <kdtree++/kdtree.hpp>

namespace neurocid {

inline Coord centerComponent(const Object* node, const int& k) {
	if(k == 0)
		return node->loc_.x_;

	return node->loc_.y_;
}

template<typename Tunit> Coord perfDescComponent(const Tunit* node, const int& k) {
	return node->perfDesc_[k];
}


class ObjectBsp : public KDTree::KDTree<2, Object*, std::pointer_to_binary_function<const Object*, const int&, Coord>> {
public:
	typedef std::vector<Object*> NodeVector;

	ObjectBsp() : KDTree::KDTree(std::ptr_fun(centerComponent)) {
	}
};

template <size_t TnumDesc, typename Tunit> class PerfDescBsp : public KDTree::KDTree<TnumDesc, Tunit*, std::pointer_to_binary_function<const Tunit*, const int&, Coord>> {
public:
	typedef std::vector<PerfDescBsp*> NodeVector;
	typedef KDTree::KDTree<TnumDesc, Tunit*, std::pointer_to_binary_function<const Tunit*, const int&, Coord>> Base;

	PerfDescBsp() : KDTree::KDTree<TnumDesc, Tunit*, std::pointer_to_binary_function<const Tunit*, const int&, Coord>>(std::ptr_fun(perfDescComponent<Tunit>)) {
	}

	std::pair<typename Base::const_iterator, typename Base::distance_type> findClosestMate(Tunit* t) {
		auto result = this->find_nearest_if(t, std::numeric_limits<Coord>().max(), [&](Tunit* candidate) {
			return candidate != t;
		});
		CHECK(t != (*result.first));

		return result;
	}

	std::vector<Tunit*> findInRange(Tunit* t, Coord range) {
		std::vector<Tunit*> result;
		this->find_within_range(t, range, std::back_inserter(result));
		return result;
	}
};
}

#endif
