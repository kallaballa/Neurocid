CXX      := g++-4.8
CXXFLAGS := -std=c++0x -pedantic -Wall `pkg-config --cflags SDL_gfx sdl SDL_image SDL_ttf` 
LDFLAGS  := -L/opt/local/lib 
LIBS     := -lm `pkg-config --libs SDL_gfx sdl SDL_image SDL_ttf` -lfann -lBox2D -lX11
.PHONY: all release debug clean distclean 

ifndef NO_OPENMP
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

debug: CXXFLAGS += -g3 -O0 -rdynamic -D_CHECK_BRAIN_ALLOC
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
	tests/tests
