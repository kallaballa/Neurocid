#ifndef SCREEN_HPP_
#define SCREEN_HPP_

#include "battlefield.hpp"
#include "widgets.hpp"

namespace neurocid {

class Screen : public Container, public gcn::ActionListener {
public:
	virtual void update(BattleField& field) = 0;
	virtual bool isForm() = 0;
};


} /* namespace neurocid */

#endif /* SCREEN_HPP_ */
