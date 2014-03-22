/*
 * scanner1.cpp
 *
 *  Created on: Mar 14, 2014
 *      Author: elchaschab
 */

#include "scanner.hpp"
#include "population.hpp"
#include "battlefield.hpp"
#include "bsp.hpp"
#include "util.hpp"

namespace tankwar {

pair<Vector2D, Coord> ClusterScanner::findNearestCenter(const vector<Vector2D>& centers, const Vector2D& loc) {
	Coord distance = NO_COORD;
	Vector2D nearest = NO_VECTOR2D;

	for(const Vector2D& c : centers) {
		Coord candidate = c.distance(loc);
		if(candidate < distance) {
			distance = candidate;
			nearest = c;
		}
	}

	assert(nearest != NO_VECTOR2D);
	assert(distance != NO_COORD);

	return {nearest,distance};
}

void ClusterScanner::scanClusterCenters(Population& team, vector<Vector2D>& result, size_t numCenters) {
	int	stages = 20;		// number of stages

	KMterm term(100, 0, 0, 0,		// run for 100 stages
			0.10,			// min consec RDL
			0.10,			// min accum RDL
			3,			// max run stages
			0.50,			// init. prob. of acceptance
			10,			// temp. run length
			0.95);			// temp. reduction factor

    term.setAbsMaxTotStage(stages);		// set number of stages

    size_t teamSize = team.size();
	KMdata dataPts(2, std::max(teamSize, numCenters));		// allocate data storage
	KMdataArray pts = dataPts.getPts();
    if(teamSize < numCenters) {
    	size_t n = 0;
    	while(n < numCenters) {
			for(size_t i = 0;  i < team.size(); ++i) {
				result.push_back(team[i].loc_);

				if(++n == numCenters)
					break;
			}
    	}
    } else {
		for(size_t i = 0;  i < team.size(); ++i) {
			Tank& t = team[i];
			pts[i][0] = t.loc_.x;
			pts[i][1] = t.loc_.y;
		}

	    dataPts.buildKcTree();			// build filtering structure
	    KMfilterCenters ctrs(numCenters, dataPts);		// allocate centers

/*	    KMlocalLloyds kmLloyds(ctrs, term);		// repeated Lloyd's
	    ctrs = kmLloyds.execute();			// execute
*/

	    cout << "\nExecuting Clustering Algorithm: Swap\n";
	    KMlocalSwap kmSwap(ctrs, term);		// Swap heuristic
	    ctrs = kmSwap.execute();

/*	    cout << "\nExecuting Clustering Algorithm: EZ-Hybrid\n";
	    KMlocalEZ_Hybrid kmEZ_Hybrid(ctrs, term);	// EZ-Hybrid heuristic
	    ctrs = kmEZ_Hybrid.execute();

	    cout << "\nExecuting Clustering Algorithm: Hybrid\n";
	    KMlocalHybrid kmHybrid(ctrs, term);		// Hybrid heuristic
	    ctrs = kmHybrid.execute();
*/

	    for (size_t i = 0; i < numCenters; i++) {
	    	result.push_back(Vector2D(ctrs[i][0], ctrs[i][1]));
	    }
	   // std::cerr << "### stages: " << kmHybrid.getTotalStages() << std::endl;
    }
    assert(result.size() == numCenters);
}

void ClusterScanner::teamScan(Population& active, Population& passive, vector<Vector2D>& ctrFriends, vector<Vector2D>& ctrEnemies, BattleFieldLayout& bfl) {
	for(size_t i = 0; i < active.size(); ++i) {
		Tank& t = active[i];
		if(t.dead_)
			continue;

		Scan& scan = t.scan_;
		scan.dir_ = t.getDirection();
		scan.scale_ = {bfl.width_, bfl.height_};
		scan.loc_ = t.loc_;
		scan.objects_.clear();
		for(Vector2D cf : ctrFriends) {
			scan.objects_.push_back(ScanObject{
				FRIEND,
				cf,
				t.distance(cf)
			});
		}

		for(Vector2D ce : ctrEnemies) {
			scan.objects_.push_back(ScanObject{
				ENEMY,
				ce,
				t.distance(ce)
			});
		}

		for(Projectile* p : t.projectiles_) {
			if(p->dead_)
				continue;

			p->scan_.dir_ = p->getDirection();
			p->scan_.loc_ = p->loc_;
			auto result = findNearestCenter(ctrEnemies, p->loc_);
			Vector2D enemyLoc = result.first;
			assert(p->scan_.objects_.empty() || p->scan_.objects_.size() == 2);

			if(result.second != NO_COORD) {
				if(p->scan_.objects_.empty()) {
					p->scan_.objects_.push_back(ScanObject{
						ENEMY,
						enemyLoc,
						result.second
					});
				} else if(result.second < p->scan_.objects_[0].dis_){
					p->scan_.objects_[0].loc_ = enemyLoc;
					p->scan_.objects_[0].dis_ = result.second;
				}
			} else {
				p->scan_.objects_.push_back(ScanObject{
					ENEMY,
					NO_VECTOR2D,
					NO_COORD
				});
			}

			result = findNearestCenter(ctrFriends, p->loc_);
			Vector2D friendLoc = result.first;

			if(result.second != NO_COORD) {
				if(p->scan_.objects_.size() == 1) {
					p->scan_.objects_.push_back(ScanObject{
						FRIEND,
						friendLoc,
						result.second
					});
				} else if(result.second < p->scan_.objects_[0].dis_){
					p->scan_.objects_[1].loc_ = friendLoc;
					p->scan_.objects_[1].dis_ = result.second;
				}
			} else {
				p->scan_.objects_.push_back(ScanObject{
					FRIEND,
					NO_VECTOR2D,
					NO_COORD
				});
			}
		}
	}
}

void ClusterScanner::scan(BattleField& field) {
	assert(field.teams_.size() == 2);

	Population& teamA = field.teams_[0];
	Population& teamB = field.teams_[1];
	centersA_.clear();
	centersB_.clear();
	scanClusterCenters(teamA, centersA_, 3);
	scanClusterCenters(teamB, centersB_, 3);
	assert(centersA_.size() == 3);
	assert(centersB_.size() == 3);
/*
	std::cerr << "############## scan" << std::endl;
	std::cerr << "tanksA: ";
	for(Tank& t : teamA) {
		std::cerr << t.loc_ << std::endl;
	}
	std::cerr <<  std::endl;
	std::cerr << "tanksB: ";
	for(Tank& t : teamB) {
		std::cerr << t.loc_ << std::endl;
	}
	std::cerr <<  std::endl;

	std::cerr << "centersA: ";
	for (Vector2D c : centersA_) {
		std::cerr << c << "\t";
	}
	std::cerr << std::endl;
	std::cerr << "centersB: ";
	for (Vector2D c : centersB_) {
		std::cerr << c << "\t";
	}
	std::cerr << std::endl;
*/
	teamScan(teamA, teamB, centersA_, centersB_, field.layout_);
	teamScan(teamB, teamA, centersB_, centersA_, field.layout_);
}

} /* namespace tankwar */
