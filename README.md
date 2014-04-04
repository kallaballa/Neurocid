# Neurocid
#### A space ship simulation controlled by neural networks, evolved through a genetic algorithm

### Build
All dependencies except ffmpeg are included in the repo. if you have problems with the ffmpeg support you may disable video capturing functionality entirely by prefixing make with WITHOUT_VIDEOENC (e.g. WITHOUT_VIDEOENC=1 make)

    git clone https://github.com/kallaballa/Neurocid.git
    cd Neurocid
    ./buildDeps.sh
    make
    
### Run
There are two shell scripts that are examples of how to run neurocid to train and watch space ship populations.
basic_training.sh - attempts to produce a well fighting population in a short time.
play.sh - loads a population and multiplies it 5 times to run it it in a big scenario.
You might have to tweak hardcoded parameters in the scripts (e.g. resolution).

    
