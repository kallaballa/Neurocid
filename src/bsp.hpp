#ifndef BSB_H_
#define BSB_H_

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

	PerfDescBsp() : KDTree::KDTree<TnumDesc, Tunit*, std::pointer_to_binary_function<const Tunit*, const int&, Coord>>(std::ptr_fun(perfDescComponent<Tunit>)) {
	}

	Tunit* findClosestMate(Tunit* t) {
		auto result = this->find_nearest_if(t, std::numeric_limits<Coord>().max(), [&](Tunit* candidate) {
			return candidate != t;
		});
		assert(t != (*result.first));

		if(result.second == std::numeric_limits<Coord>().max())
			return t; //there is no one else
		else
			return (*result.first);
	}
};
}

#endif
