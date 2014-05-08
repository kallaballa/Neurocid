#ifndef HELP_WIDGET_HPP_
#define HELP_WIDGET_HPP_

#include "gui.hpp"
#include "widgets.hpp"
#include "form.hpp"
#include <string>

namespace neurocid {

using std::string;

class HelpScreen : public Form {
	OsdWidget* helpText_;
	Button* closeButton_;
public:
	HelpScreen();

	~HelpScreen() {
		delete helpText_;
	}

	virtual void update(BattleField& field);
	virtual void open();
	virtual void action(const gcn::ActionEvent& event);

	virtual bool isForm() override {
		return true;
	}
};
} /* namespace neurocid */

#endif /* OPTIONS_WIDGET_HPP_ */
