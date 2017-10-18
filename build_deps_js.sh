#/bin/bash

cd guichan; make clean; rm -r CMakeFiles; rm CMakeCache.txt
cd ../box2d/; make clean; rm -r CMakeFiles; rm CMakeCache.txt
cd ../fann/; make clean;  rm -r CMakeFiles; rm CMakeCache.txt
make -C ../kmlocal/ clean

cd ../guichan; CC=emcc CXX=em++ cmake  -DSDL_IMAGE_LIBRARY=/usr/lib64/libSDL_image.so -DSDL_LIBRARY_TEMP=/usr/lib64/libSDL.so -DSDLMAIN_LIBRARY=/usr/lib64/libSDL.so  .; make
cd ../box2d; CC=emcc CXX=em++ cmake -DBOX2D_BUILD_SHARED=ON -DBOX2D_BUILD_STATIC=OFF -DBOX2D_BUILD_EXAMPLES=OFF -DBOX2D_INSTALL=OFF .; make 
cd ../fann; CC=emcc CXX=em++ cmake .; make
cd ../kmlocal; make clean; make CC=emcc CXX=em++
cd ../LuaJIT-2.0.3; make clean; make CC=emcc CXX=em++

