#include "population.hpp"
#include "placer.hpp"
#include "util.hpp"

using namespace tankwar;
int main(int argc, char** argv) {
	PopulationLayout pl_;
	vector<Population> teams = makeTeams(2, 20, pl_);
  
	for (Population& team : teams) {
		for (Tank& t : team) {
			/*t.loc_;
			t.rotation_;
			t.range_;
			t.explode_;
			t.dead_;*/
			//render here
			for (Projectile& p : t.projectiles_) {
				/*p.loc_;
				p.rotation_;
				p.range_;
				p.explode_;
				p.dead_;*/
				//render here
			}
		}
	}
	return 0;
}

