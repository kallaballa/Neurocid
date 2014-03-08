#include "population.hpp"
#include "placer.hpp"
#include "util.hpp"

using namespace tankwar;
int main(int argc, char** argv) {
	vector<Population> teams = makeTeams(2, 20, {4,3,3,1});
  
	for (Population& team : teams) {
		for (Tank& t : team) {
			t.loc_;
			t.dir_;
			t.rotation_;
			t.range_;
			t.explode_;
			t.dead_;
			//render here
			for (Projectile& p : t.projectiles_) {
				p.loc_;
				p.dir_;
				p.rotation_;
				p.range_;
				p.explode_;
				p.dead_;
				//render here
			}
		}
	}
	return 0;
}

