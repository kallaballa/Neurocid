#include "population.hpp"
#include "frontend.hpp"
#include "eventloop.hpp"
#include "procedural_scenarios.hpp"
#include "2d.hpp"
#include "error.hpp"
#include "gui/guichangui.hpp"
#include "sdl/sdlcanvas.hpp"
#include "sound_encoder.hpp"
#ifdef _JAVASCRIPT
#include "/home/elchaschab/build/emsdk-portable/emscripten/1.37.22/system/include/emscripten.h"
#endif

#include <X11/Xlib.h>

#ifndef _NO_JSON
#include "json_scenario.hpp"
#endif

#ifndef _NO_THREAD
#include <thread>
#endif

#ifndef _NO_PROGRAM_OPTIONS
#include <boost/program_options.hpp>
#endif

#ifndef _NO_PROGRAM_OPTIONS
namespace po = boost::program_options;
#endif

namespace nc = neurocid;

using std::string;
using std::cerr;
using std::endl;
using std::vector;

int main(int argc, char** argv) {
  XInitThreads();
  //command line parsing
  string loadAFile;
  string saveAFile;
  string loadBFile;
  string saveBFile;
  string captureFile;
  string brainDumpFileA;
  string brainDumpFileB;
  string scenarioName;
  size_t gameIterations = 1000;
  size_t width = 800;
  size_t height = 800;
  size_t frameRate = 25;
  long autosaveInterval = 0;

#ifndef _NO_PROGRAM_OPTIONS
  po::options_description genericDesc("Options");
  genericDesc.add_options()
      ("iterations,i", po::value<size_t>(&gameIterations), "Run n iterations of the game")
      ("autosave,a", po::value<long>(&autosaveInterval), "Automatically save teams every n minutes")
      ("loadA", po::value<string>(&loadAFile), "Load the population as team A from a file before running the scenario")
      ("loadB", po::value<string>(&loadBFile), "Load the population as team B from a file before running the scenario")
      ("saveA", po::value<string>(&saveAFile), "Save the team A population to a file after running the scenario")
      ("saveB", po::value<string>(&saveBFile), "Save the team B population to a file after running the scenario")
      ("capture,c", po::value<string>(&captureFile), "Capture the game to a video file")
      ("brainDumpA", po::value<string>(&brainDumpFileA), "Dump the neural network output of team A to an AU (audio) file.")
      ("brainDumpB", po::value<string>(&brainDumpFileB), "Dump the neural network output of team B to an AU (audio) file.")
      ("width,x", po::value<size_t>(&width), "The window width")
      ("height,y", po::value<size_t>(&height), "The window height")
      ("framerate,f", po::value<size_t>(&frameRate), "The frame rate of the renderer and video encoder")
      ("help,h", "Produce help message");

  po::options_description hidden("Hidden options");
  hidden.add_options()("scenario", po::value<string>(&scenarioName), "scenario");

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

  srand(time(0));
  /* Let's load the scenario.
   * Either we load a hard coded (procedural) scenario
   * or if a filename is given we load a json scenario
   */
#else
  scenarioName = "SymmetricLinesFar";
#endif

  //initialize frontend subsystems subsystems - core, canvas, gui
  nc::init_core(width, height, frameRate);

  string suffix = ".nsj";
  nc::Scenario* scenario = NULL;

  if (scenarioName.length() > suffix.length() && std::equal(suffix.rbegin(), suffix.rend(), scenarioName.rbegin())) {
#ifndef _NO_JSON
    scenario = new nc::json::JsonScenario(scenarioName);
#endif
  } else {
    // this has to be done once before trying to access procedural scenarios.
    nc::load_procedural_scenarios();
    // get a declarative scenario by name
    scenario = nc::get_procedural_scenario(scenarioName);

    CHECK_MSG(scenario != NULL, "Unknown Scenario");
  }
  CHECK_MSG(scenario != NULL, "Unable to load a Scenario");

#ifndef _NO_GUI
  nc::SDLCanvas* sdlc = new nc::SDLCanvas(width, height, scenario->bfl_);
  nc::GuiChanGui* gui = new nc::GuiChanGui(sdlc->getSurface());

  nc::init_canvas(sdlc);
  nc::init_gui(gui);
  nc::init_video_capture(captureFile);
#endif

  //make default layouts
  nc::PopulationLayout pl = nc::make_default_population_layout();
  nc::GeneticLayout gl = nc::make_default_genetic_layout();

  //make default genetic pools
  vector<nc::GeneticPool> pools = nc::make_pools(2, gl);

  vector<nc::Population> teams(2);

  //either load or create a team
  if (!loadAFile.empty()) {
    ifstream is(loadAFile);
    nc::read_population(0, teams[0], is);
  } else {
    teams[0] = nc::make_population(0, pl);
  }

  if (!loadBFile.empty()) {
    ifstream is(loadBFile);
    nc::read_population(1, teams[1], is);
  } else {
    teams[1] = nc::make_population(1, pl);
  }
#ifndef _NO_SOUNDENC
  nc::SoundEncoder::getInstance()->init("teamA", brainDumpFileA, teams[0].layout_.bl_.numOutputs, 1000);
  nc::SoundEncoder::getInstance()->init("teamB", brainDumpFileB, teams[1].layout_.bl_.numOutputs, 1000);
#endif

#ifndef _NO_THREADS
  std::thread gameThread([&]() {
#endif
      teams[0].score_ = 0;
      teams[1].score_ = 0;
#ifdef _JAVASCRIPT
      neurocid::init_game(gameIterations, scenario, teams, pools);
      emscripten_set_main_loop(neurocid::iter_game, 0, 1);
#else

      //play the game!
      neurocid::play_game(gameIterations, scenario, teams, pools, captureFile, autosaveInterval);

      //save the result if requested
      if(!saveAFile.empty()) {
        ofstream os(saveAFile);
        write_population(teams[0],os);
      }

      if(!saveBFile.empty()) {
        ofstream os(saveBFile);
        write_population(teams[1],os);
      }
#endif
#ifndef _NO_THREADS
    });
#endif

#ifndef _NO_THREADS
  //event loop
  std::thread eventThread([&]() {
    nc::EventLoop el;

    while(neurocid::is_running()) {
      el.process();
    }
  });

  while (neurocid::is_running()) {
    neurocid::render();
  }

  eventThread.join();
  gameThread.join();
#endif
  nc::quit();
}
