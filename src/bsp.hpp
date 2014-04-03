#ifndef BSB_H_
#define BSB_H_

#include "object.hpp"
#include <kdtree++/kdtree.hpp>

namespace tankwar {

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
};
}

#endif
