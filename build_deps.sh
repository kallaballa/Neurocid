#/bin/bash

cd guichan; make clean; rm -r CMakeFiles; rm CMakeCache.txt
cd ../box2d/; make clean; rm -r CMakeFiles; rm CMakeCache.txt
cd ../fann/; make clean;  rm -r CMakeFiles; rm CMakeCache.txt
make -C ../kmlocal clean
make -C ../json_spirit clean
make -C ../LuaJIT-2.0.3 clean

cd ../guichan; cmake  -DENABLE_OPENGL=OFF -DBUILD_GUICHAN_OPENGL_SHARED=OFF  .; make -j8
cd ../box2d; cmake -DBOX2D_BUILD_SHARED=ON -DBOX2D_BUILD_STATIC=OFF -DBOX2D_BUILD_EXAMPLES=OFF -DBOX2D_INSTALL=OFF .; make -j8 
cd ../fann; cmake .; make -j8
make -C ../kmlocal  -j8
make -C ../json_spirit -j8
make -C ../LuaJIT-2.0.3 -j8
