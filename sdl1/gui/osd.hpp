#ifndef OSD_HPP_
#define OSD_HPP_

#include "gui.hpp"
#include "screen.hpp"
#include "widgets.hpp"
#include <string>

namespace neurocid {

using std::string;

class OsdScreenWidget : public Screen {
	OsdWidget* osdStatistics_;
	OsdWidget* osdTracker_;
	OsdWidget* osdStatus_;
	OsdWidget* osdDeadA_;
	OsdWidget* osdDeadB_;

	OsdWidget* osdScoreA_;
	OsdWidget* osdScoreB_;

	Button* helpButton_;
public:
	OsdScreenWidget();

	~OsdScreenWidget() {
		delete osdStatistics_;
		delete osdTracker_;
		delete osdStatus_;
	}

	void update(BattleField& field) override;

	void showOsdStatistics(bool s) {
		osdStatistics_->setVisible(s);
	}

	void showOsdTracker(bool s) {
		osdTracker_->setVisible(s);
	}

	void showOsdStatus(bool s) {
		osdStatus_->setVisible(s);
	}

	bool isOsdStatisticsVisible() {
		return osdStatistics_->isVisible();
	}

	bool isOsdTrackerVisible() {
		return osdTracker_->isVisible();
	}

	bool isOsdStatusVisible() {
		return osdStatus_->isVisible();
	}

	virtual void action(const gcn::ActionEvent& event);

	virtual bool isForm() override {
		return false;
	}
};


} /* namespace neurocid */

#endif /* OSD_HPP_ */
