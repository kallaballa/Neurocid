#/bin/bash

cd guichan; make clean; rm -r CMakeFiles; rm CMakeCache.txt
cd ../box2d/; make clean; rm -r CMakeFiles; rm CMakeCache.txt
cd ../fann/; make clean;  rm -r CMakeFiles; rm CMakeCache.txt
make -C ../kmlocal clean
make -C ../json_spirit clean
make -C ../LuaJIT-2.0.3 clean

cd ../guichan; cmake -DCMAKE_CXX_FLAGS="-m32" -DENABLE_OPENGL=OFF -DBUILD_GUICHAN_OPENGL_SHARED=OFF  .; make
cd ../box2d; cmake -DCMAKE_CXX_FLAGS="-m32" -DBOX2D_BUILD_SHARED=ON -DBOX2D_BUILD_STATIC=OFF -DBOX2D_BUILD_EXAMPLES=OFF -DBOX2D_INSTALL=OFF .; make 
cd ../fann; cmake  -DCMAKE_C_FLAGS="-m32" .; make
make -C ../kmlocal LDFLAGS="-m32" CFLAGS="-m32"
make -C ../json_spirit CXXFLAGS="-m32" LDFLAGS="-m32 -shared"
make -C ../LuaJIT-2.0.3 CC="gcc -m32"
