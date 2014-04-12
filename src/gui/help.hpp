#ifndef HELP_WIDGET_HPP_
#define HELP_WIDGET_HPP_

#include "gui.hpp"
#include "widgets.hpp"
#include "form.hpp"
#include <string>

namespace neurocid {

using std::string;

class BattleField;

class HelpScreen : public NeurocidWidget<gcn::Window>, public Form, gcn::ActionListener {
	OsdWidget* helpText_;
	Button* closeButton_;

	static HelpScreen* instance_;
	HelpScreen();
public:
	static HelpScreen* getInstance() {
		if(instance_ == NULL)
		instance_ = new HelpScreen();
		return instance_;
	}

	~HelpScreen() {
		delete helpText_;
	}

	virtual void open();
	virtual void action(const gcn::ActionEvent& event);
};
} /* namespace neurocid */

#endif /* OPTIONS_WIDGET_HPP_ */
