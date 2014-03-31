CXX      := g++-4.8
CXXFLAGS := -std=c++0x -pedantic -Wall -I../kmlocal-1.7.2/src/ -I../fann/src/include -I../box2d/  `pkg-config --cflags SDL_gfx sdl SDL_image SDL_ttf` 
LDFLAGS  := -L/opt/local/lib -L../kmlocal-1.7.2/src/ -L../fann/src/ -L../box2d/Box2D
LIBS     := -lboost_system -lboost_serialization -lboost_program_options -lklocal -lm `pkg-config --libs SDL_gfx sdl SDL_image SDL_ttf` -lfann -lBox2D -lX11
.PHONY: all release info debug clean distclean 
NVCC     := /usr/local/cuda/bin/nvcc
NVCC_HOST_CXX := g++-4.6
NVCC_CXXFLAGS := -Xcompiler -fpic -I/usr/local/cuda-5.0/samples/common/inc/

ifdef JAVASCRIPT
CXX			 := em++
CXXFLAGS += -I/usr/local/include -D_NO_SERIALIZE -D_NO_PROGRAM_OPTIONS
endif

ifdef WITH_OPENMP
CXXFLAGS += -fopenmp 
LIBS     += -fopenmp
endif

ifdef NO_ASSERT
CXXFLAGS += -DNDEBUG
LIBS     += -DNDEBUG
endif

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
 LDFLAGS += -L/opt/X11/lib/
 CXXFLAGS += —stdlib=c++ 
else
 CXXFLAGS += -march=native
endif

all: release

release: LDFLAGS += -s
release: CXXFLAGS += -g0 -O3
release: dirs

info: CXXFLAGS += -g3 -O3
info: LDFLAGS += -Wl,--export-dynamic -rdynamic
info: dirs

debug: CXXFLAGS += -g3 -O0 -D_CHECK_BRAIN_ALLOC -Werror
debug: LDFLAGS += -Wl,--export-dynamic -rdynamic
debug: dirs

clean: dirs

distclean: dirs

export LDFLAGS
export CXXFLAGS
export LIBS

dirs:
	${MAKE} -C src/ ${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" ${MAKECMDGOALS}
	${MAKE} -C game/ ${MAKEFLAGS} CXX=${CXX} ${MAKECMDGOALS}
	${MAKE} -C tests/ ${MAKEFLAGS} CXX=${CXX} ${MAKECMDGOALS}
	${MAKE} -C render/ ${MAKEFLAGS} CXX=${CXX} ${MAKECMDGOALS}
	./run.sh tests/tests
