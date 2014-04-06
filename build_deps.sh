cd guichan; cmake .; make
cd ../box2d; cmake -DBOX2D_BUILD_SHARED=ON -DBOX2D_BUILD_STATIC=OFF -DBOX2D_BUILD_EXAMPLES=OFF -DBOX2D_INSTALL=OFF .; make 
cd ../fann; cmake .; make
make -C ../kmlocal-1.7.2 

