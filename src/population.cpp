
#include "population.hpp"

namespace neurocid {

PopulationLayout make_default_population_layout() {
	return {
		40, // size_
		//Ship Layout
		{
		  //Projectile Layout
			{
			  //Blast Layout
			  {
			     1000, // radius
			     10    // maxColaterl
			  },
				1,    // max_speed_
				30000, // max_travel_
				10     // radius_
			},
			false, // isDummy_
			true,// canShoot_
			true,// canRotate_
			true,// canMove_
			true,// canJump_
			false,// disableProjectileFitness_

			50.0,// radius_
			1.0, // max_speed_
			1.0, // max_rotation_
			20000.0, // max_fuel_
			20000.0, // start_fuel_
			100.0, // jump_rate_
			1.0, // fuel_rate_
			0.7, // hardness_

			20,// max_ammo_
			0, // start_ammo_
			5, // max_cooldown_
			200, //max_stun_
			1, // max_damage_
			1, // crashes_per_damage_
			4,  // num_perf_desc_
			"fitness/amir" // fitnessFunction;
		},
		//BrainLayout
		{
			10,  // metaInputs
			3,  // metaLayers
			5,  // meta neurons per hidden layer
		  57,// inputs
			6,  // outputs
			5,  // layers
			11, // neurons per hidden layer
			8   // numBrains
		},
		//FacilityLayout
		{
			6000,
			500,
			500,
			1,
			2000000
		}
	};
}

}
