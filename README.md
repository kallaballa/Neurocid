# Neurocid
#### A space ship simulation controlled by neural networks, evolved through a genetic algorithm

### Prerequisites
Installing development packages for following libraries is mandatory: **pkg-config, boost-system, boost-program-options, SDL-gfx, sdl, SDL-image, SDL-ttf**
Following libraries can be disabled by prefixing make with WITHOUT_VIDEOENC=1 (e.g WITHOUT_VIDEOENC=1 make): **libavformat, libavcodec, libswscale, libavutil**
Following libraries can be disabled by prefixing make with WITHOUT_SERIALIZE=1: **boost-serialization**

On Debian/Ubuntu you can install required packages like this (Video encoding not supported):

    apt-get install pkg-config libsdl1.2-dev libsdl-gfx1.2-dev libsdl-image1.2-dev libsdl-ttf1.2-dev libboost-system-dev libboost-program_options-dev libboost-serialization-dev 

**NOTE:** Because of troubles with different ffmpeg implementations video encoding is not supported on debian/ubunut. don't forget to use WITHOUT_VIDEOENC

### Build
All dependencies except ffmpeg are included in the repo. if you have problems with the ffmpeg support you may disable video capturing functionality entirely by prefixing make with WITHOUT_VIDEOENC

    git clone https://github.com/kallaballa/Neurocid.git
    cd Neurocid
    ./buildDeps.sh
    make
    
### Run
There are two shell scripts that are examples of how to run neurocid to train and watch space ship populations.
* basic_training.sh - attempts to produce a well fighting population in a short time.
* play.sh - loads a population and multiplies it 5 times to run it it in a big scenario.

**NOTE:** You might have to tweak hardcoded parameters in the scripts (e.g. resolution).

    
