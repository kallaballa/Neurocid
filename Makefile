CXX      := g++-4.8
CXXFLAGS := -std=c++0x -pedantic -Wall -I../kmlocal-1.7.2/src/ `pkg-config --cflags SDL_gfx sdl SDL_image SDL_ttf` 
LDFLAGS  := -L/opt/local/lib -L../kmlocal-1.7.2/src/
LIBS     := -lboost_system -lboost_serialization -lboost_program_options -lklocal -lm `pkg-config --libs SDL_gfx sdl SDL_image SDL_ttf` -lfann -lBox2D -lX11
.PHONY: all release info debug clean distclean 
NVCC     := /usr/local/cuda/bin/nvcc
NVCC_HOST_CXX := g++-4.6
NVCC_CXXFLAGS := -Xcompiler -fpic -I/usr/local/cuda-5.0/samples/common/inc/

ifdef WITH_OPENMP
CXXFLAGS += -fopenmp 
LIBS     += -fopenmp
endif

ifdef NO_ASSERT
CXXFLAGS += -DNDEBUG
LIBS     += -DNDEBUG
endif

ifeq ($(UNAME), Darwin)
 CXXFLAGS +=  -stdlib=libc++
else
 CXXFLAGS += -march=native
endif

all: release

release: LDFLAGS += -s
release: CXXFLAGS += -g0 -O3
release: dirs

info: CXXFLAGS += -g3 -O3 -rdynamic
info: LDFLAGS += -Wl,--export-dynamic
info: dirs

debug: CXXFLAGS += -g3 -O0 -rdynamic -D_CHECK_BRAIN_ALLOC
debug: LDFLAGS += -Wl,--export-dynamic
debug: dirs

clean: dirs

distclean: dirs

export LDFLAGS
export CXXFLAGS
export LIBS

dirs:
	${MAKE} -C src/ ${MAKEFLAGS} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" ${MAKECMDGOALS}
	${MAKE} -C game/ ${MAKEFLAGS} ${MAKECMDGOALS}
	${MAKE} -C tests/ ${MAKEFLAGS} ${MAKECMDGOALS}
	${MAKE} -C render/ ${MAKEFLAGS} ${MAKECMDGOALS}
	./run.sh tests/tests
