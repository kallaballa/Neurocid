#ifndef WIDGETS_HPP_
#define WIDGETS_HPP_

#include "gui.hpp"
#include <string>

namespace neurocid {

using std::string;

class OsdWidget : public NeurocidWidget<gcn::TextBox> {
public:
	OsdWidget();
	void clear();
	void addLine(const string& key, const string& value);
	void addLine(const string& value);
};

class Button : public NeurocidWidget<gcn::Button> {
public:
	Button(const string& s);
};
}
#endif /* WIDGETS_HPP_ */
