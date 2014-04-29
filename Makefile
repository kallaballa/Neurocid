CXX      := g++
CXXFLAGS := -fno-strict-aliasing -std=c++0x -pedantic -Wall -I../kmlocal/src/ -I../fann/src/include -I../box2d/ -I../guichan/include/ -I../LuaJIT-2.0.3/src/ `pkg-config --cflags sdl SDL_image SDL_ttf` 
LDFLAGS  := -L/opt/local/lib -L../kmlocal/src/ -L../fann/src/ -L../box2d/Box2D -L../guichan/ -L../libnoise/lib -L../json_spirit/ -L../LuaJIT-2.0.3/src/
LIBS     := -lklocal -lm -lfann -lguichan -lguichan_sdl -lBox2D -ljson_spirit -lluajit `pkg-config --libs sdl SDL_image SDL_ttf`
.PHONY: all release debian-release info debug clean debian-clean distclean 
NVCC     := /usr/local/cuda/bin/nvcc
NVCC_HOST_CXX := g++-4.6
NVCC_CXXFLAGS := -Xcompiler -fpic -I/usr/local/cuda-5.0/samples/common/inc/
DESTDIR := /
PREFIX := /usr/local
MACHINE := $(shell uname -m)

ifeq ($(MACHINE), x86_64)
  LIBDIR = lib64
endif
ifeq ($(MACHINE), i686)
  LIBDIR = lib
endif

ifdef JAVASCRIPT
CXX			 := em++
CXXFLAGS += -I/usr/local/include
WITHOUT_SERIALIZE=1
WITHOUT_VIDEOENC=1
WITHOUT_THREADS=1
WITHOUT_PROGRAM_OPTS=1
WITHOUT_POOL_ALLOC=1
WITHOUT_JSON=1
WITHOUT_EVENTLOOP=1
WITHOUT_SDLGFX=1
endif

ifdef WITHOUT_SDLGFX
CXXFLAGS += -D_NO_SDLGFX
else
CXXFLAGS += `pkg-config --cflags SDL_gfx`
LIBS +=  `pkg-config --libs SDL_gfx`
endif

ifdef WITHOUT_EVENTLOOP
CXXFLAGS += -D_NO_EVENTLOOP
endif

ifdef WITHOUT_JSON
CXXFLAGS += -D_NO_JSON
endif

ifdef WITH_OPENMP
CXXFLAGS += -fopenmp 
LIBS     += -fopenmp
endif

ifdef WITOUT_POOL_ALLOC
CXXFLAGS += -D_NO_BOOST_ALLOC
endif

ifdef WITHOUT_SERIALIZE
CXXFLAGS += -D_NO_SERIALIZE
else
LIBS     += -lboost_system -lboost_serialization
endif

ifdef WITHOUT_VIDEOENC
CXXFLAGS += -D_NO_VIDEOENC
else
CXXFLAGS += -D__STDC_CONSTANT_MACROS `pkg-config --cflags libavformat libavcodec libswscale libavutil`
LIBS     += `pkg-config --libs libavformat libavcodec libswscale libavutil`
endif

ifdef WITHOUT_THREADS
CXXFLAGS += -D_NO_THREADS
endif

ifdef WITHOUT_PROGRAM_OPTS
CXXFLAGS += -D_NO_PROGRAM_OPTIONS
else
LIBS += -lboost_system -lboost_program_options
endif

ifdef WITHOUT_ASSERT
CXXFLAGS += -DNDEBUG
LIBS     += -DNDEBUG
endif

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
 LDFLAGS += -L/opt/X11/lib/ -pagezero_size 10000 -image_base 100000000
 CXXFLAGS += —stdlib=c++ 
else
 CXXFLAGS += -march=native
endif

all: release

release: LDFLAGS += -s
release: CXXFLAGS += -g0 -O3
release: dirs

info: CXXFLAGS += -g3 -O0
info: LDFLAGS += -Wl,--export-dynamic -rdynamic
info: dirs

debug: CXXFLAGS += -g3 -O0
debug: LDFLAGS += -Wl,--export-dynamic -rdynamic
debug: dirs

profile: CXXFLAGS += -g3 -O1
profile: LDFLAGS += -Wl,--export-dynamic -rdynamic
profile: dirs

clean: dirs

export LDFLAGS
export CXXFLAGS
export LIBS

dirs:
	${MAKE} -C src/ ${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" ${MAKECMDGOALS}
	${MAKE} -C game/ ${MAKEFLAGS} CXX=${CXX} ${MAKECMDGOALS}
ifndef JAVASCRIPT
	${MAKE} -C tests/ ${MAKEFLAGS} CXX=${CXX} ${MAKECMDGOALS}
endif
#	./run.sh tests/tests

debian-release:
	${MAKE} -C src/ -${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" release
	${MAKE} -C game/ -${MAKEFLAGS} CXX=${CXX} release
	${MAKE} -C tests/ -${MAKEFLAGS} CXX=${CXX} release

debian-clean:
	${MAKE} -C src/ -${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" clean
	${MAKE} -C game/ -${MAKEFLAGS} CXX=${CXX} clean
	${MAKE} -C tests/ -${MAKEFLAGS} CXX=${CXX} clean

install: ${TARGET}
	mkdir -p ${DESTDIR}/${PREFIX}/${LIBDIR}/neurocid
	cp -r lua/ ${DESTDIR}/${PREFIX}/${LIBDIR}/neurocid/	
	cp LuaJIT-2.0.3/src/libluajit.so*  ${DESTDIR}/${PREFIX}/${LIBDIR}/neurocid/libluajit-5.1.so.2
	cp json_spirit/libjson_spirit.so* ${DESTDIR}/${PREFIX}/${LIBDIR}/neurocid
	cp kmlocal/src/libklocal.so* ${DESTDIR}/${PREFIX}/${LIBDIR}/neurocid
	cp fann/src/libfann.so* ${DESTDIR}/${PREFIX}/${LIBDIR}/neurocid
	cp guichan/libguichan*.so* ${DESTDIR}/${PREFIX}/${LIBDIR}/neurocid
	cp box2d/Box2D/libBox2D.so* ${DESTDIR}/${PREFIX}/${LIBDIR}/neurocid
	cp src/libneurocid.so ${DESTDIR}/${PREFIX}/${LIBDIR}/neurocid
	cp game/neurocid-bin ${DESTDIR}/${PREFIX}/${LIBDIR}/neurocid
	cp -r examples/ ${DESTDIR}/${PREFIX}/${LIBDIR}/neurocid
	cp *.ttf ${DESTDIR}/${PREFIX}/${LIBDIR}/neurocid
	mkdir -p ${DESTDIR}/${PREFIX}/bin
	cp neurocid ${DESTDIR}/${PREFIX}/bin
	cp neurocid-dbg ${DESTDIR}/${PREFIX}/bin
	mkdir -p ${DESTDIR}/etc/
	echo "export NEUROCID_PATH=\"/${PREFIX}/${LIBDIR}/neurocid\"" > ${DESTDIR}/etc/neurocid

distclean:
	rm -r ${PREFIX}/${LIBDIR}/neurocid/
	rm /etc/neurocid
	rm ${PREFIX}/bin/neurocid

plot:
	gnuplot < graphTeamA.gnuplot > teamA.png
	gnuplot < graphTeamB.gnuplot > teamB.png

