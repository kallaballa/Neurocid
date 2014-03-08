#include <population.hpp>
#include <placer.hpp>

namespace tankwar {

using std::vector;

Population makePopulation(size_t teamID, size_t size, BrainLayout layout) {
	Population p;
	for(size_t i = 0; i < size; i++) {
		Tank t(teamID, layout, {0, 0}, 0);
		t.brain_.randomize();
		p.push_back(t);
	}
	return p;
}

vector<Population> makeTeams(size_t numTeams, size_t teamSize, BrainLayout l) {
	vector<Population> teams(numTeams);
	size_t teamID = 0;
	std::generate(teams.begin(), teams.end(), [&]() { return makePopulation(teamID++, teamSize, l); });
	return teams;
}

}

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

