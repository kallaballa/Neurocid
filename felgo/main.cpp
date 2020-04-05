#include <QApplication>
#include <FelgoApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <cassert>

#include "../src/population.hpp"
#include "../src/frontend.hpp"
#include "../sdl1/procedural_scenarios.hpp"
#include "../src/2d.hpp"
#include "../src/error.hpp"
#include "felgocanvas.hpp"
#include "../src/canvas.hpp"
#include "../sdl1/json_scenario.hpp"
#include "qneurocidcontrol.hpp"

namespace nc = neurocid;

using std::string;
using std::cerr;
using std::endl;
using std::vector;

QNeurocidControl NC_CONTROL;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    FelgoApplication felgo;
    QQmlApplicationEngine engine;
    felgo.initialize(&engine);
    felgo.setMainQmlFileName(QStringLiteral("qml/Main.qml"));
    engine.load(QUrl(felgo.mainQmlFileName()));

    QList<QObject*> rootObjs = engine.rootObjects();
    assert(rootObjs.size() == 1);

    QObject *qmlGfxCanvas = rootObjs.first()->findChild<QObject*>("gfxCanvas");
    QObject *qmlApp = rootObjs.first();
    assert(qmlGfxCanvas != nullptr);
    assert(qmlApp != nullptr);

    QObject::connect(qmlApp, SIGNAL(zoomIn()),
                     &NC_CONTROL, SLOT(zoomIn()));
    QObject::connect(qmlApp, SIGNAL(zoomOut()),
                     &NC_CONTROL, SLOT(zoomOut()));
    QObject::connect(qmlApp, SIGNAL(left()),
                     &NC_CONTROL, SLOT(left()));
    QObject::connect(qmlApp, SIGNAL(right()),
                     &NC_CONTROL, SLOT(right()));
    QObject::connect(qmlApp, SIGNAL(up()),
                     &NC_CONTROL, SLOT(up()));
    QObject::connect(qmlApp, SIGNAL(down()),
                     &NC_CONTROL, SLOT(down()));
    QObject::connect(qmlApp, SIGNAL(tiltUp()),
                     &NC_CONTROL, SLOT(tiltUp()));
    QObject::connect(qmlApp, SIGNAL(tiltDown()),
                     &NC_CONTROL, SLOT(tiltDown()));
    QObject::connect(qmlApp, SIGNAL(setSpeed(int)),
                     &NC_CONTROL, SLOT(setSpeed(int)));
    QObject::connect(qmlApp, SIGNAL(setPaused(bool)),
                     &NC_CONTROL, SLOT(setPaused(bool)));
    QObject::connect(qmlApp, SIGNAL(dumpTeams()),
                     &NC_CONTROL, SLOT(dumpTeams()));

    std::thread renderThread([=]() {
        while(!qmlGfxCanvas->property("available").toBool()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cerr << "wait" << std::endl;
        }

        //default parameters
        string saveAFile = "SymmetricLinesAttackerMove.pop";
        string scenarioName = "SymmetricLinesAttackerMove";
        size_t gameIterations = 1000;
        size_t frameRate = 25;
        nc::Coord width = 1920;
        nc::Coord height = 1080;
        long autosaveInterval = 10;

        //seed the pseudo-random generator
        srand(static_cast<unsigned int>(time(nullptr)));

        //initialize core components of the fronend
        nc::init_core(width, height, frameRate);

        nc::Scenario* scenario = nullptr;

        // this has to be done once before trying to access procedural scenarios.
        nc::load_procedural_scenarios();
        // get a procedural scenario by name
        scenario = nc::get_procedural_scenario(scenarioName);

        CHECK_MSG(scenario != nullptr, "Unable to load a Scenario")

        nc::FelgoGFX* felgoGfx = new nc::FelgoGFX(qmlGfxCanvas);
        nc::FelgoCanvas* felgoCanvas = new nc::FelgoCanvas(felgoGfx, width, height, scenario->bfl_);
        nc::Canvas* canvas = dynamic_cast<nc::Canvas*>(felgoCanvas);
        CHECK_MSG(canvas != nullptr, "Can't cast FelgoCanvas to Canvas!")

        //initialize rendering
        nc::init_canvas(canvas);

        //make default layouts
        nc::PopulationLayout pl = nc::make_default_population_layout();
        nc::GeneticLayout gl = nc::make_default_genetic_layout();

        //make default genetic pools
        vector<nc::GeneticPool> pools = nc::make_pools(2, gl);

        //create teams
        vector<nc::Population> teams(2);
        teams[0] = nc::make_population(0, pl);
        teams[1] = nc::make_population(1, pl);

        std::thread gameThread([&]() {
            teams[0].score_ = 0;
            teams[1].score_ = 0;

            //play the game!
            neurocid::play_game(gameIterations, scenario, teams, pools, "", autosaveInterval);
        });

        while (neurocid::is_running()) {
            neurocid::render();
        }

        gameThread.join();
        nc::quit();
    });

    renderThread.detach();

    return app.exec();
}
