#/bin/bash

cd guichan; make clean; rm -r CMakeFiles; rm CMakeCache.txt
cd ../box2d/; make clean; rm -r CMakeFiles; rm CMakeCache.txt
cd ../fann/; make clean;  rm -r CMakeFiles; rm CMakeCache.txt
make -C ../kmlocal clean
make -C ../json_spirit clean
make -C ../LuaJIT-2.0.3 clean

cd ../guichan; cmake -DENABLE_OPENGL=OFF -DBUILD_GUICHAN_SDL_SHARED=OFF -DBUILD_GUICHAN_SHARED=OFF  .; make
cd ../box2d; cmake -DBOX2D_BUILD_SHARED=OFF -DBOX2D_BUILD_STATIC=ON -DBOX2D_BUILD_EXAMPLES=OFF -DBOX2D_INSTALL=OFF .; make 
cd ../fann; cmake  .; make
ar -rcs ../fann/src/libfann.a ../fann/src/CMakeFiles/floatfann.dir/floatfann.c.o
make -C ../kmlocal LDFLAGS="-shared -static-libgcc -Wl,-Bstatic" CFLAGS="-fPIC"
ar rcs ../kmlocal/src/libkmlocal.a ../kmlocal/src/*o
make -C ../json_spirit LDFLAGS="-static"
ar -rcs ../json_spirit/libjson_spirit.a ../json_spirit/*o
make -C ../LuaJIT-2.0.3 CC="gcc"
