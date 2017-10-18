CXX      := g++
CXXFLAGS := -fno-strict-aliasing -std=c++0x -pedantic -Wall -I../kmlocal/src/ -I../fann/src/include -I../box2d/
LDFLAGS  := -L/opt/local/lib -L../kmlocal/src/ -L../fann/src/ -L../box2d/Box2D -L../json_spirit/ 
LIBS     := -lklocal -lm -lfann -lBox2D -ljson_spirit
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
CXXFLAGS += -D_JAVASCRIPT -s DEMANGLE_SUPPORT=1 -s ALLOW_MEMORY_GROWTH=1
LDFLAGS +=  -O3 -s ALLOW_MEMORY_GROWTH=1 --embed-file DejaVuSansMono-Bold.ttf  --embed-file DejaVuSansMono.ttf --embed-file lua --embed-file vector2d.lua --embed-file ncutils.lua
WITHOUT_STACKTRACE=1
WITHOUT_SERIALIZE=1
WITHOUT_VIDEOENC=1
WITHOUT_THREADS=1
WITHOUT_PROGRAM_OPTS=1
WITHOUT_POOL_ALLOC=1
WITHOUT_JSON=1
WITHOUT_EVENTLOOP=1
WITHOUT_SDLGFX=1
WITHOUT_SOUNDENC=1
WITHOUT_LUAJIT=1
endif

ifdef X86
CXXFLAGS += -m32
LDFLAGS += -L/usr/lib -m32 
endif

ifdef STATIC
LDFLAGS += -static-libgcc -Wl,-Bstatic
endif

ifdef X86
CXXFLAGS += -m32
LDFLAGS += -L/usr/lib -static-libgcc -m32 -Wl,-Bstatic
endif 

ifdef WITHOUT_SDLGFX
CXXFLAGS += -D_NO_SDLGFX
else
CXXFLAGS += `pkg-config --cflags SDL_gfx SDL_image`
LIBS +=  `pkg-config --libs SDL_gfx SDL_image`
endif

ifdef WITHOUT_SOUNDENC
CXXFLAGS += -D_NO_SOUNDENC
endif

ifdef WITHOUT_GUI
CXXFLAGS += -D_NO_GUI
WITHOUT_EVENTLOOP=1
endif

ifdef WITHOUT_LUAJIT
CXXFLAGS += -I../lua-5.3.4/src/ -D_NO_LUAJIT -DLUA_COMPAT_5_1
LDFLAGS += -L../lua-5.3.4/src/
LIBS += -llua
else
CXXFLAGS +=  -I../LuaJIT-2.0.3/src/
LDFLAGS += -L../LuaJIT-2.0.3/src/
LIBS += -lluajit
endif

ifdef WITHOUT_STACKTRACE
CXXFLAGS += -D_NO_STACKTRACE
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

ifdef WITHOUT_SNDFILE
CXXFLAGS += -D_NO_SNDFILE
else
CXXFLAGS += `pkg-config --cflags sndfile`
LIBS     += `pkg-config --libs sndfile`
endif

ifdef WITHOUT_THREADS
CXXFLAGS += -D_NO_THREADS
endif

ifdef WITHOUT_PROGRAM_OPTS
CXXFLAGS += -D_NO_PROGRAM_OPTIONS
else
LIBS += -lboost_system -lboost_program_options
endif

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
 LDFLAGS += -L/opt/X11/lib/
else
 CXXFLAGS += -march=native
endif

all: release

ifneq ($(UNAME_S), Darwin)
release: LDFLAGS += -s
endif
release: CXXFLAGS += -g0 -O3
release: dirs

info: CXXFLAGS += -g3 -O0
info: LDFLAGS += -Wl,--export-dynamic -rdynamic
info: dirs

debug: CXXFLAGS += -g3 -O0 -rdynamic
debug: LDFLAGS += -Wl,--export-dynamic -rdynamic
debug: dirs

profile: CXXFLAGS += -g3 -O1
profile: LDFLAGS += -Wl,--export-dynamic -rdynamic
profile: dirs

hardcore: CXXFLAGS += -g0 -Ofast -DNDEBUG
ifeq ($(UNAME_S), Darwin)
hardcore: LDFLAGS += -s
endif
hardcore: dirs

clean: dirs

export LDFLAGS
export CXXFLAGS
export LIBS

dirs:
	${MAKE} -C src/ ${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" ${MAKECMDGOALS}
ifndef STATIC
	${MAKE} -C sdl1/ ${MAKEFLAGS} CXX=${CXX} ${MAKECMDGOALS}
#	${MAKE} -C sdl2/ ${MAKEFLAGS} CXX=${CXX} ${MAKECMDGOALS}
ifndef JAVASCRIPT
	${MAKE} -C tests/ ${MAKEFLAGS} CXX=${CXX} ${MAKECMDGOALS}
endif
endif

debian-release:
	${MAKE} -C src/ -${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" release
	${MAKE} -C sdl1/ -${MAKEFLAGS} CXX=${CXX} release
	${MAKE} -C sdl2/ -${MAKEFLAGS} CXX=${CXX} release
	${MAKE} -C tests/ -${MAKEFLAGS} CXX=${CXX} release

debian-clean:
	${MAKE} -C src/ -${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" clean
	${MAKE} -C sdl1/ -${MAKEFLAGS} CXX=${CXX} clean
	${MAKE} -C sdl2/ -${MAKEFLAGS} CXX=${CXX} clean
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
	cp sdl1/neurocid-bin ${DESTDIR}/${PREFIX}/${LIBDIR}/neurocid
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

