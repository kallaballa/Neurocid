#ifndef BSB_H_
#define BSB_H_

#include "object.hpp"
#include <kdtree++/kdtree.hpp>

namespace tankwar {

inline Coord centerComponent(const Object* node, const int& k) {
	assert(k >= 0 && k <= 1);
	if(k == 0)
		return node->loc_.x;

	return node->loc_.y;
}


class Bsp : public KDTree::KDTree<2, Object*, std::pointer_to_binary_function<const Object*, const int&, Coord>> {
public:
	typedef std::vector<Object*> NodeVector;

	Bsp() : KDTree::KDTree(std::ptr_fun(centerComponent)) {
	}
};
}
#endif
