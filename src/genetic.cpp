#include "genetic.hpp"
#include "fann.h"
#include "util.hpp"
#include "battlefieldlayout.hpp"
#include "error.hpp"
#include <algorithm>
#include <iostream>
#include <set>

namespace neurocid {

GeneticLayout make_default_genetic_layout() {
  return {
    0.05, // mutationRate
    0.7,// crossoverRate
    4,// crossoverIterations
    0.3,// maxPertubation
    4,// numElite
    1,// numEliteCopies
    true// usePerfDesc_
  };
}

using std::set;

GeneticPool::GeneticPool(GeneticLayout params) :
    layout_(params) {
  initialized_ = true;
}

GeneticPool::GeneticPool() :
    layout_() {
  initialized_ = false;
}

/*
 * mutates a chromosome by perturbing its weights by an amount not greater than params_.maxPerturbation_
 */
void GeneticPool::mutate(Brain& brain) {
  //traverse the chromosome and mutate each weight dependent
  //on the mutation rate

  for (size_t b = 0; b < brain.layout_.numBrains_ + 1; ++b) {
    fann_type* weights = brain.weights(b);
    for (size_t i = 0; i < brain.size(b); ++i) {
      //do we perturb this weight?
      if (fRand(0, 1) < layout_.mutationRate) {
        //add or subtract a small value to the weight
        weights[i] += ((fRand(0, 1) - fRand(0, 1)) * layout_.maxPertubation);
      }
    }
  }
}

/*
 * Returns a ship base on roulette wheel sampling
 */
Ship& GeneticPool::pickSpecimen(Population& pop) {
  double minFitness = 99999999999;
  for (size_t i = 0; i < pop.size(); ++i) {
    minFitness = std::min(minFitness, pop[i].fitness_);
  }

  //generate a random number between 0 & total fitness count
  double slice = (double) fRand(0, pop.stats_.totalFitness_ - (minFitness * pop.size()));

  double fitnessSoFar = 0;
  for (size_t i = 0; i < pop.size(); ++i) {
    fitnessSoFar += (pop[i].fitness_ - minFitness);

    //if the fitness so far > random number return the tank at this point
    if (fitnessSoFar >= slice) {
      return pop[i];
    }
  }

  CHECK(false);
  return pop[0]; 	//surpress warning
}

/*
 * Returns a ship based on roulette wheel sampling
 */
Ship* GeneticPool::pickSpecimen(vector<Ship*>& pop) {
  double totalFitness = 0;
  double minFitness = 99999999999;

  for (Ship* s : pop) {
    totalFitness += s->fitness_;
    minFitness = std::min(minFitness, s->fitness_);
  }

  //generate a random number between 0 & total fitness count
  double slice = (double) (fRand(0, totalFitness - (minFitness * pop.size())));

  //this will be set to the chosen tank go through the tanks adding up the fitness so far
  double fitnessSoFar = 0;

  for (size_t i = 0; i < pop.size(); ++i) {
    fitnessSoFar += (pop[i]->fitness_ - minFitness);

    //if the fitness so far > random number return the tank at this point
    if (fitnessSoFar >= slice) {
      return pop[i];
    }
  }

  CHECK(false);
  return pop[0]; 	//surpress warning
}

/*
 * With a chance defined by params_.crossoverRate_ perform a crossover of brain_.weights()
 */
std::pair<Ship, Ship> GeneticPool::crossover(Ship &mum, Ship &dad, size_t iterations) {
  Ship baby1 = mum.makeChild();
  Ship baby2 = mum.makeChild();

  for (size_t b = 0; b < mum.brain_->layout_.numBrains_ + 1; ++b) {
    fann_type* wMum = mum.brain_->weights(b);
    fann_type* wDad = dad.brain_->weights(b);
    fann_type* wBaby1 = baby1.brain_->weights(b);
    fann_type* wBaby2 = baby2.brain_->weights(b);

    if ((fRand(0, 1) > layout_.crossoverRate) || (mum == dad)) {
      for (size_t i = 0; i < mum.brain_->size(b); ++i) {
        wBaby1[i] = wMum[i];
        wBaby2[i] = wDad[i];
      }
      continue;
    }

    size_t last_cp = 0;
    bool cross = false;

    //generate up to "iterations" crossover points and perform them in ascending order
    set<size_t> crossPoints;

    for (size_t i = 0; i < iterations; ++i) {
      crossPoints.insert(iRand(0, mum.brain_->size(b) - 1));
    }

    for (const size_t& cp : crossPoints) {
      //create the offspring
      for (size_t j = last_cp; j < cp; ++j) {
        if (cross) {
          wBaby1[j] = wMum[j];
          wBaby2[j] = wDad[j];
        } else {
          wBaby1[j] = wDad[j];
          wBaby2[j] = wMum[j];
        }
      }

      last_cp = cp;
      cross = !cross;
    }

    for (size_t i = last_cp; i < mum.brain_->size(b); ++i) {
      if (cross) {
        wBaby1[i] = wMum[i];
        wBaby2[i] = wDad[i];
      } else {
        wBaby1[i] = wDad[i];
        wBaby2[i] = wMum[i];
      }
    }
  }

  return {baby1, baby2};
}

/*
 * copy numCopies copies of the n best specimen into the out population
 */
void GeneticPool::copyNBest(size_t n, const size_t numCopies, Population& in, Population& out) {
  //add the required amount of copies of the n most fittest to the supplied population
  while (n--) {
    for (size_t i = 0; i < numCopies; ++i) {
      Ship& t = in[(in.size() - 1) - n];
      Ship clone = t.clone();
      clone.isElite = true;
      out.push_back(clone);
    }
  }
}

/*
 * generates the statistics for the given population
 */
void GeneticPool::calculateStatistics(Population& pop) {

  size_t size = pop.size();
  double highestSoFar = 0;
  double lowestSoFar = 9999999;

  pop.stats_.bestFitness_ = std::numeric_limits<double>().max();

  for (size_t i = 0; i < size; ++i) {
    pop.stats_.totalFriendlyFire_ += pop[i].friendlyFire_;
    pop.stats_.totalCrash_ += pop[i].crash_;
    pop.stats_.totalDefensiveHits_ += pop[i].defensiveHits_;
    pop.stats_.totalOffensiveHits_ += pop[i].offensiveHits_;
    pop.stats_.totalDamage_ += pop[i].damage_;
    pop.stats_.totalRecharged_ += pop[i].recharged_;
    pop.stats_.totalBrainSwitches_ += pop[i].brain_->brainStats_.countSwitches();

    //update fittest if necessary
    if (highestSoFar == 0 || pop[i].fitness_ > highestSoFar) {
      highestSoFar = pop[i].fitness_;
      pop.stats_.fittestGenome_ = i;
      pop.stats_.bestFitness_ = highestSoFar;
    }

    //update worst if necessary
    if (pop[i].fitness_ < lowestSoFar) {
      lowestSoFar = pop[i].fitness_;
      pop.stats_.worstFitness_ = lowestSoFar;
    }

    pop.stats_.totalFitness_ += pop[i].fitness_;
  }

  CHECK(pop.stats_.bestFitness_ != std::numeric_limits<double>().max());

  pop.stats_.averageFitness_ = pop.stats_.totalFitness_ / size;
  pop.stats_.averageFriendlyFire_ = pop.stats_.totalFriendlyFire_ / size;
  pop.stats_.averageCrash_ = pop.stats_.totalCrash_ / size;
  pop.stats_.averageDefensiveHits_ = pop.stats_.totalDefensiveHits_ / size;
  pop.stats_.averageOffensiveHits_ = pop.stats_.totalOffensiveHits_ / size;
  pop.stats_.averageDamage_ = pop.stats_.totalDamage_ / size;
  pop.stats_.averageRecharged_ = pop.stats_.totalRecharged_ / size;
  pop.stats_.averageBrainSwitches_ = pop.stats_.totalBrainSwitches_ / pop.stats_.battleFieldIterations_ + 1;
}

/*
 * Use the genetic algorithm to construct a new population from the old
 */
Population GeneticPool::epoch(Population& old_pop, const BattleFieldLayout& bfl) {
  if (!initialized_ || old_pop.size() == 1) {
    Population new_pop = old_pop;
    new_pop.clear();
    old_pop.stats_.reset();

    for (Ship& t : old_pop) {
      t.calculateFitness(bfl, old_pop.facilities_.front(), old_pop.winner_);
      t.isElite = false;
    }

    //sort the population (for scaling and elitism)
    sort(old_pop.begin(), old_pop.end());

    //calculate best, worst, average and total fitness
    calculateStatistics(old_pop);

    for (Ship& t : old_pop) {
      new_pop.push_back(t.makeChild());
    }
    new_pop.stats_ = old_pop.stats_;
    return new_pop;
  }

  Population new_pop = old_pop;
  new_pop.clear();
  old_pop.stats_.reset();

  for (Ship& t : old_pop) {
    t.calculateFitness(bfl, old_pop.facilities_.front(), old_pop.winner_);
    t.isElite = false;
  }

  //sort the population (for scaling and elitism)
  sort(old_pop.begin(), old_pop.end());

  //calculate best, worst, average and total fitness
  calculateStatistics(old_pop);

  /*
   * Only winning teams get to have an elite by copying
   * some of the fittest genomes without any mutation/crossover.
   * Make sure we add an EVEN number or the roulette wheel sampling will crash
   * NOTE: we don't dont copy elites if we use perf descriptors since that is already a form of elitism
   */
  if (old_pop.winner_ && !layout_.usePerfDesc_ && layout_.numElite_ < old_pop.size()) {
    if (!(layout_.numEliteCopies_ * (layout_.numElite_ % 2))) {
      copyNBest(layout_.numElite_, layout_.numEliteCopies_, old_pop, new_pop);
    }
  }

  CHECK(old_pop.layout_.sl_.numPerfDesc_ == 4);
  PerfDescBsp<4, Ship> pdb;
  if (layout_.usePerfDesc_) {
    for (Ship& t : old_pop) {
      CHECK(t.perfDesc_.size() == 4);
      pdb.insert(&t);
    }
  }
  //now we enter the GA loop
  //repeat until a new population is generated
  while (new_pop.size() < old_pop.size()) {
    //grab two chromosones
    Ship& mum = pickSpecimen(old_pop);
    Ship* dad;
    if (layout_.usePerfDesc_) {
      CHECK(old_pop.size() > 2);
      auto pair = pdb.findClosestMate(&mum);
      Ship* closest = *pair.first;
      Coord dist = pair.second;
      if (dist == 0.0)
        dist = 0.01;

      vector<Ship*> result;

      Coord range = dist * 1.5;

      do {
        result = pdb.findInRange(&mum, range);
        range *= 1.5;
      } while (result.size() < 2);

      /*for(Ship* s : result) {
       std::cerr << s->perfDesc_[0] << '\t' << s->perfDesc_[2] << '\t' << s->perfDesc_[3] << '\t' << s->perfDesc_[4] << std::endl;
       }*/

      if (result.empty()) {
        dad = closest;
      } else {
        //std::cerr << "dist/range:" << dist << "\t" << range << std::endl;
        std::sort(result.begin(), result.end(), [&](const Ship* s1, const Ship* s2) {
          return s1->fitness_ < s2->fitness_;
        });

        dad = pickSpecimen(result);
      }
    } else
      dad = &pickSpecimen(old_pop);

    //create some offspring via crossover
    std::pair<Ship, Ship> babies = crossover(mum, *dad, layout_.crossoverIterations);

    //now we mutate
    mutate(*babies.first.brain_);
    mutate(*babies.second.brain_);

    //now copy into vecNewPop population
    new_pop.push_back(babies.first);
    if (new_pop.size() < old_pop.size())
      new_pop.push_back(babies.second);
  }
  CHECK(new_pop.size() == old_pop.size());
  old_pop.stats_.generationCnt_++;
  new_pop.stats_ = old_pop.stats_;
  return new_pop;
}
}
