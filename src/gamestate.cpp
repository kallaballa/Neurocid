#include "gamestate.hpp"
#include "game.hpp"

namespace neurocid {

GameState* GameState::instance_ = NULL;

void GameState::setCurrentGame(Game* g) {
  currentGame_ = g;
}

Game* GameState::getCurrentGame() {
  return currentGame_;
}

void GameState::dumpTeams() {
  dumpMutex.lock();
  pause();
#ifndef _NO_THREADS
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
#endif
  Game* game = getCurrentGame();
  if (game != NULL) {
    time_t rawtime;
    struct tm * timeinfo;
    char bufferA[80];
    char bufferB[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(bufferA, 80, "dumpA_%d-%m-%Y_%I:%M:%S.pop", timeinfo);
    strftime(bufferB, 80, "dumpB_%d-%m-%Y_%I:%M:%S.pop", timeinfo);

    string filenameA(bufferA);
    string filenameB(bufferB);

    ofstream osa(filenameA);
    write_population(game->teams_[0], osa);

    ofstream osb(filenameB);
    write_population(game->teams_[1], osb);
  }
  resume();
  dumpMutex.unlock();
}
}
