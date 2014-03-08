CXX      := g++-4.8
CXXFLAGS := -fopenmp -DETLOG -std=c++0x -pedantic -Wall `pkg-config --cflags SDL_gfx sdl SDL_image` -D_CHECK_BRAIN_ALLOC
LDFLAGS  := -L/opt/local/lib 
LIBS     := -fopenmp -lm `pkg-config --libs SDL_gfx sdl SDL_image` -lfann -lX11
DIRS     := src tests
.PHONY: all release debug clean distclean 

ifeq ($(UNAME), Darwin)
 CXXFLAGS +=  -stdlib=libc++
else
 CXXFLAGS += -march=native
endif

all: release

release: LDFLAGS += -s
release: CXXFLAGS += -g0 -Ofast 
release: dirs

debug: CXXFLAGS += -g3 -O0 -rdynamic
debug: LDFLAGS += -Wl,--export-dynamic
debug: dirs

clean: dirs

distclean: dirs

export LDFLAGS
export CXXFLAGS
export LIBS

dirs:
	${MAKE} -C src/ ${MAKEFLAGS} ${MAKECMDGOALS}
	${MAKE} -C game/ ${MAKEFLAGS} ${MAKECMDGOALS}
	${MAKE} -C tests/ ${MAKEFLAGS} ${MAKECMDGOALS}
	${MAKE} -C render/ ${MAKEFLAGS} ${MAKECMDGOALS}

