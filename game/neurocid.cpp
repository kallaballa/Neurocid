#include "population.hpp"
#include "renderer.hpp"
#include "game.hpp"
#include "gamestate.hpp"
#include "time_tracker.hpp"
#include "scenario.hpp"
#include "canvas.hpp"
#include "gui/gui.hpp"
#include "gui/osd.hpp"
#include "eventloop.hpp"
#include "declarative_scenarios.hpp"

#ifndef _NO_VIDEOENC
#include "video_encoder.hpp"
#endif

#ifndef _NO_THREAD
#include <thread>
#endif

#ifndef _NO_PROGRAM_OPTIONS
#include <boost/program_options.hpp>
#endif

using namespace neurocid;

#ifndef _NO_PROGRAM_OPTIONS
namespace po = boost::program_options;
#endif

using std::string;
using std::cerr;
using std::endl;
using std::vector;

void playGame(size_t gameIter, Scenario* scenario, vector<Population>& teams, vector<GeneticPool>& pools, const string& videoFile) {
	GameState& gs = *GameState::getInstance();
	TimeTracker& tt = *TimeTracker::getInstance();

#ifndef _NO_VIDEOENC
	VideoEncoder& ve = *VideoEncoder::getInstance();
	if(!videoFile.empty())
		ve.init(videoFile.c_str(), AV_CODEC_ID_H264);
#endif
	while(gs.isRunning() && --gameIter > 0) {
		tt.execute("game", [&](){
			Game game(scenario, teams, pools);
			gs.setCurrentGame(&game);
			teams = game.play(true);
			gs.setCurrentGame(NULL);
		});
	}
#ifndef _NO_VIDEOENC
	if(!videoFile.empty())
		ve.close();
#endif
	gs.stop();
}

int main(int argc, char** argv) {
	srand(time(0));
	loadScenarios();
	PopulationLayout pl = {
		//Ship Layout
		{
		    //Projectile Layout
			{
				1,    // max_speed
				10000, // max_travel
				5     // range
			},
			false, // isDummy
			true,// canShoot
			true,// canRotate
			true,// canMove
			false,// disableProjectileFitness

			50.0,// range_
			1, // max_speed_
			1, // max_rotation
			5, // max_cooldown
			5, // max_ammo_
			6, // max_damage_

			1, // crashes_per_damage_
			4  // num_perf_desc_
		},
		//BrainLayout
		{
		    83, // inputs
			5,  // outputs
			8,  // layers
			11  // neurons per hidden layer
		}
	};

	GeneticParams gp = {
			0.1, // mutationRate
			0.7, // crossoverRate
			1,   // crossoverIterations
			0.3, // maxPertubation
			4,   // numElite
			1,   // numEliteCopies
			false// usePerfDesc_
	};

	string loadFile;
	string saveFile;
	string captureFile;
	string scenarioName;
	bool save = false;
	bool load = false;
	Scenario* scenario = NULL;
	size_t gameIterations = 1000;
	size_t multiply = 0;
	size_t width = 800;
	size_t height = 800;
	size_t frameRate = 25;
	vector<Population> teams;
	vector<GeneticPool> pools(2);
	pools[0] = GeneticPool(gp);
	pools[1] = GeneticPool(gp);

#ifndef _NO_PROGRAM_OPTIONS
	po::options_description genericDesc("Options");
	genericDesc.add_options()
		("iterations,i", po::value< size_t >(&gameIterations), "Run n iterations of the game")
		("multiply,m", po::value< size_t >(&multiply), "Multiply the number of tanks in the populations")
		("load,l", po::value< string >(&loadFile), "Load the population from a file before running the scenario")
		("save,s", po::value< string >(&saveFile), "Save the population to a file after running the scenario")
		("capture,c", po::value< string >(&captureFile), "Capture the game to a video file")
		("width,x", po::value< size_t >(&width), "The window width")
		("height,y", po::value< size_t >(&height), "The window height")
		("framerate,f", po::value< size_t >(&frameRate), "The frame rate of the renderer and video encoder")
		("help,h", "Produce help message");

    po::options_description hidden("Hidden options");
    hidden.add_options()
    	("scenario", po::value< string >(&scenarioName), "scenario");

	po::options_description cmdline_options;
    cmdline_options.add(genericDesc).add(hidden);

    po::positional_options_description p;
    p.add("scenario", -1);

    po::options_description visible;
    visible.add(genericDesc);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cerr << "Usage: neurocid [options]\n";
        std::cerr << visible;
        return 0;
    }

    load = vm.count("load");
    save = vm.count("save");
    assert(vm.count("scenario"));
	scenario = getScenario(scenarioName);
#else
    scenario = getScenario("SymmetricLinesFar");
#endif
    assert(scenario != NULL);

    Options& opt = *Options::getInstance();
	opt.WINDOW_WIDTH = width;
    opt.WINDOW_HEIGHT = height;
    opt.FRAMERATE = frameRate;

    GameState& gs = *GameState::getInstance();
	Canvas& canvas = *Canvas::getInstance();
	TimeTracker& timeTracker = *TimeTracker::getInstance();

	timeTracker.setEnabled(true);

	gs.setSlow(false);
    gs.setSlower(false);

    canvas.enableDrawGrid(false);
    canvas.enableDrawEngines(true);

	// initialize osd overlay
	Gui::init(canvas.getSurface());
	OsdScreenWidget::init(canvas.width(),canvas.height());
	Gui& gui = *Gui::getInstance();
	OsdScreenWidget& osd = *OsdScreenWidget::getInstance();
	gui.setScreen(&osd);

    if(load) {
    	ifstream is(loadFile);
    	read_teams(teams,is);
    } else {
    	teams = makeTeams(2,20, pl);
    }

    if(multiply > 1) {
    	multiplyTeams(teams, multiply);
    }

#ifndef _NO_THREADS
    std::thread gameThread([&]() {
#endif
    	teams[0].score_ = 0;
    	teams[1].score_ = 0;
    	playGame(gameIterations, scenario, teams, pools, captureFile);
        if(save) {
      	  if(teams[0].size() > 20)
      		  teams[0].resize(20);

      	  if(teams[1].size() > 20)
      		  teams[1].resize(20);

        	ofstream os(saveFile);
        	write_teams(teams,os);
        }
#ifndef _NO_THREADS
	});
#endif

#ifndef _NO_THREADS
    EventLoop el;
    Renderer& renderer = *Renderer::getInstance();
    while(gs.isRunning()) {
    	el.process();
    	renderer.render();
    }

	gameThread.join();
#endif

	SDL_Quit();
}
