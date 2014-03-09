#include "brain.hpp"
#include "population.hpp"
#include "tank.hpp"
#include <fann.h>
#include <iostream>
#include <assert.h>

namespace tankwar {

#ifdef _CHECK_BRAIN_ALLOC
std::map<fann*, size_t> Brain::nnAllocs_;
size_t Brain::nnAllocCnt_ = 0;
#endif

Brain::Brain(BrainLayout layout): layout_(layout), nn_(NULL) {
	assert(layout_.numLayers_ >= 3);
	assert(layout_.numLayers_ < 20);
	unsigned int layerArray[layout_.numLayers_];
	layerArray[0] = layout_.numInputs;
	layerArray[layout_.numLayers_ - 1] = layout_.numOutputs;

	for(size_t i = 1; i < (layout_.numLayers_ - 1); i++) {
		layerArray[i] = layout_.neuronsPerHidden;
	}

	nn_ = fann_create_standard_array(layout_.numLayers_, layerArray);
    fann_set_activation_function_hidden(nn_, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(nn_, FANN_SIGMOID_SYMMETRIC);
#ifdef _CHECK_BRAIN_ALLOC
    size_t id = ++nnAllocCnt_;
    nnAllocs_[nn_] = id;
    std::cerr << "alloc: " << id << std::endl;
#endif
}

Brain::Brain(const Brain& other): layout_(other.layout_), nn_(other.nn_) {
}

Brain::~Brain() {
}

void Brain::destroy() {
    assert(nn_ != NULL);
#ifdef _CHECK_BRAIN_ALLOC
	auto it = nnAllocs_.find(nn_);
	assert(it != nnAllocs_.end());
	std::cerr << "free: " << (*it).second << std::endl;
	nnAllocs_.erase(it);
#endif
	fann_destroy(nn_);
	nn_ = NULL;
	destroyed_ = true;
}

void Brain::randomize() {
	fann_randomize_weights(nn_, -1, 1);
}

void Brain::update(const Tank& tank, Population& ownTeam, Population& otherTeam) {
	assert(nn_ != NULL);
	size_t numInputs = layout_.numInputs;
	assert(layout_.numInputs == fann_get_num_input(nn_));
	assert(layout_.numOutputs == fann_get_num_output(nn_));
	fann_type inputs[numInputs];

	Vector2D nearestProjectileLoc = {0,0};
	Coord nearestProjectileDis = std::numeric_limits<Coord>().max();

	for(const Tank& enemy : otherTeam) {
		for(const Projectile& p : enemy.projectiles_) {
			Coord distance = tank.distance(p);
			if(distance < nearestProjectileDis){
				nearestProjectileDis = distance;
				nearestProjectileLoc = p.loc_;
			}
		}
	}

	Vector2D nearestEnemyLoc = {0,0};
	Coord nearestEnemyDis = std::numeric_limits<Coord>().max();

	for(const Tank& enemy : otherTeam) {
		Coord distance = tank.distance(enemy);
		if(distance < nearestEnemyDis){
			nearestEnemyDis = distance;
			nearestEnemyLoc = enemy.loc_;
		}
	}

	Vector2D nearestFriendLoc = {0,0};
	Vector2D nearestFriendLoc2 = {0,0};
	Coord nearestFriendDis = std::numeric_limits<Coord>().max();

	for(const Tank& f : otherTeam) {
		Coord distance = tank.distance(f);
		if(distance < nearestFriendDis){
			nearestFriendDis = distance;
			nearestFriendLoc2 = nearestFriendLoc;
			nearestFriendLoc = f.loc_;
		}
	}
//	std::cerr<< tank.loc_.x << ","<< tank.loc_.y << "\t" <<nearestEnemyLoc.x << ","<< nearestEnemyLoc.y << std::endl;

	assert(nearestEnemyLoc != Vector2D(0,0));
	assert(nearestEnemyDis != std::numeric_limits<Coord>().max());
	assert(nearestFriendLoc != Vector2D(0,0));
	assert(nearestFriendDis != std::numeric_limits<Coord>().max());

	Vector2D toNearestEnemy = (nearestEnemyLoc - tank.loc_).normalize();
	Vector2D toNearestFriend = (nearestFriendLoc - tank.loc_).normalize();
	Vector2D toNearestFriend2 = (nearestFriendLoc2 - tank.loc_).normalize();
	Vector2D toNearestProjectile = (nearestProjectileLoc - tank.loc_).normalize();


//	std::cerr<< tank.dir_.x << ","<< tank.dir_.y << "\t" << toNearestEnemy.x << ","<< toNearestEnemy.y << std::endl;

	inputs[0] = (fann_type)tank.dir_.x;
	inputs[1] = (fann_type)tank.dir_.y;
	//inputs[2] = (fann_type)	(1.0 / Params::MAX_PROJECTILES) * (double)tank.projectiles_;
	inputs[2] = (fann_type)toNearestEnemy.x;
	inputs[3] = (fann_type)toNearestEnemy.y;
	//inputs[4] = (fann_type)toNearestFriend.x;
	//inputs[5] = (fann_type)toNearestFriend.y;
	//inputs[6] = (fann_type)toNearestFriend2.x;
	//inputs[7] = (fann_type)toNearestFriend2.y;
	inputs[4] = (fann_type)toNearestProjectile.x;
	inputs[5] = (fann_type)toNearestProjectile.y;
	//inputs[10] = (fann_type)1;

/*	size_t off = 3;
	for(size_t i = 0; i < ownTeam.size(); ++i) {
		inputs[off + (i*2)] = (fann_type)(tank.loc_ - ownTeam[i].loc_).normalize().x;
		inputs[off + (i*2) + 1] = (fann_type)(tank.loc_ - ownTeam[i].loc_).normalize().y;
//		inputs[off + (i*2)] = 1;
//		inputs[off + (i*2) + 1] = 1;
	}

	off += (ownTeam.size() * 2);

	for(size_t i = 0; i < otherTeam.size(); ++i) {
		inputs[off + (i*2)] = (fann_type)(tank.loc_ - otherTeam[i].loc_).normalize().x;
	    inputs[off + (i*2) + 1] = (fann_type)(tank.loc_ - otherTeam[i].loc_).normalize().y;
//		inputs[off + (i*2)] = 1;
//		inputs[off + (i*2) + 1] = 1;
	}

	off += (otherTeam.size() * 2);
*/
//	std::cerr << "input:";
	for(size_t i = 0; i < numInputs; ++i) {
		assert(!std::isnan(inputs[i]));
//		std::cerr << "\t" << inputs[i];
	}
//	std::cerr << std::endl;
	fann_type* outputs = fann_run(nn_, inputs);
	lthrust_ = outputs[0];
	rthrust_ = outputs[1];
	shoot_ = outputs[2];
	assert(!std::isnan(lthrust_) && !std::isnan(rthrust_) && !std::isnan(shoot_));

	//	std::cerr << "shoot:\t" << inputs[2] << std::endl;
//	std::cerr << "output:\t" << lthrust_ << "\t" << rthrust_ << "\t" << shoot_ << std::endl;
}

size_t Brain::size() const {
	return nn_->total_connections;
}

fann_type* Brain::weights() {
	return nn_->weights;
}
} /* namespace tankwar */
