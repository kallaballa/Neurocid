#/bin/bash

cd guichan; make clean; rm -r CMakeFiles; rm CMakeCache.txt
cd ../box2d/; make clean; rm -r CMakeFiles; rm CMakeCache.txt
cd ../fann/; make clean;  rm -r CMakeFiles; rm CMakeCache.txt
make -C ../kmlocal/ clean

cd ../guichan; CC=emcc CXX=em++ cmake .; make -j8
cd ../box2d; CC=emcc CXX=em++ cmake -DBOX2D_BUILD_SHARED=ON -DBOX2D_BUILD_STATIC=OFF -DBOX2D_BUILD_EXAMPLES=OFF -DBOX2D_INSTALL=OFF .; make -j8
cd ../fann; CC=emcc CXX=em++ cmake .; make -j8
cd ../kmlocal; make clean; make CC=emcc CXX=em++ -j8
cd ../lua-5.3.4/; make clean; make CC=emcc CXX=em++ -j8 posix

