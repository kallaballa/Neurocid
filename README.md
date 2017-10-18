# Neurocid
#### A space ship simulation controlled by neural networks, evolved through a genetic algorithm

### Prerequisites
Installing **development** packages for following libraries is mandatory: 

    pkg-config, boost-system, boost-program-options, SDL-gfx, sdl, SDL-image, SDL-ttf

Following libraries can be disabled by prefixing make with WITHOUT_VIDEOENC#1 (e.g WITHOUT_VIDEOENC#1 make):

    libavformat, libavcodec, libswscale, libavutil

Following libraries can be disabled by prefixing make with WITHOUT_SERIALIZE#1:

    boost-serialization

On Debian/Ubuntu you can install required packages like this (Video encoding not supported):

   apt-get install pkg-config libsdl1.2-dev libsdl-gfx1.2-dev libsdl-image1.2-dev libsdl-ttf2.0-dev libboost-system-dev libboost-program-options-dev libboost-serialization-dev
 
On Opensuse 13.1

    sudo zypper install libSDL-devel libSDL_image-devel libSDL_gfx-devel boost-devel libffmpeg-devel

**NOTE:** Because of troubles with different ffmpeg implementations video encoding is not supported on debian/ubuntu. don't forget to use WITHOUT_VIDEOENC

### Build
if you have problems with the ffmpeg support you may disable video capturing functionality entirely by prefixing make with WITHOUT_VIDEOENC

    git clone https://github.com/kallaballa/Neurocid.git
    cd Neurocid
    ./buildDeps.sh
    make
    sudo make install
    
### Run
In the examples directory there are two shell scripts that are examples of how to run neurocid to train and watch space ship populations.
* basic_training.sh - attempts to produce a well fighting population in a short time.
* play.sh - loads a population and multiplies it 5 times to run it it in a big scenario.

Please have a look at Hotkeys.txt to learn how to control the simulation

**NOTE:** You might have to tweak hardcoded parameters in the scripts (e.g. resolution).

### Game Overview
There are two teams consisting of space ships. Their ultimate goal is to destroy the other team which can be achieved through destroying each of the ships or destroying the enemy base. Each ship has a neural network as brain which controls all its actions. During training the teams follow a curriculum of scenarios and are evolved through genetic algorithms. Scenarios and fitness function are scriptable in lua. What you will see in the video is the final battle after the training in slow motion.

### The Rules

#### Ships
A ship can move and shoot projectiles. 
A ship has a limited amount of fuel and projectiles.
A ship dies when it is out of fuel.
A ship can collide with out ships and suffers damage.
A ship hit by a projectile suffers damage.
A ship exceeding it's maximum damage dies.
In every game scenario ships start with minimal fuel and no ammunition. They have to visit their own base to restock both.

For movement the ship has:
* Four perpendicular thrusters which can be fired all at the same time. It consumes fuel. 
* An ftl drive which can transport it instantaneously over a limited distance. It consumes a lot of fuel (way more than conventional movement would cost for the same distance).

#### Projectiles
A projectile has a limited speed but is faster than ships.
A projectile has a limited range and will explode on exceeding it.
A projectile will explode on impact. On friendly ships, enemy ships, friendly bases and enemy bases alike. 
When a projectile explodes it creates a blast. Any ship in range of the blast will be stunned for a limited time.

#### Bases
A base supplies friendly ships flying over it with fuel and ammunition
A base has a limited damage it can take before it is destroyed. Without a base the friendly team will starve since it can't restock fuel and ammunition.
 

### Legend
* Dark red dots with a trail of same color: Ships of team A moving conventionally
* Dark green dots with a trail of same color: Ships of team B moving conventionally
* Lighter red straight lines: Ships of team A moving ftl
* Lighter green straight lines: Ships of team B moving ftl
* Big lighter red dot: Base of team A
* Big lighter green dot: Base of team B
* White circle: Marks a ship which is stunned
* Orange dots: Marks a projectile blast
* Yellow dots; Projectiles of team A
* Turquoise dots: Projectiles of team B
* White bar next to bases: Health level of the base

    
