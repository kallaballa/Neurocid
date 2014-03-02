CXX     := g++-4.8
TARGET  := tankwar
SRCS    := battlefield.cpp canvas.cpp params.cpp tank.cpp tankwar.cpp brain.cpp genetic.cpp util.cpp
#precompiled headers
HEADERS := 
GCH    := ${HEADERS:.h=.gch}
OBJS    := ${SRCS:.cpp=.o} 
DEPS    := ${SRCS:.cpp=.dep} 
    
CXXFLAGS += -DETLOG -std=c++0x -pedantic -Wall `pkg-config --cflags SDL_gfx sdl SDL_image`
LDFLAGS += -L/opt/local/lib 
LIBS    += -lm `pkg-config --libs SDL_gfx sdl SDL_image` -lfann -lX11
.PHONY: all release clean distclean 

ifeq ($(UNAME), Darwin)
 CXXFLAGS +=  -stdlib=libc++
endif

all: release

release: LDFLAGS += -s
release: CXXFLAGS += -g0 -O3 
release: ${TARGET}

debug: CXXFLAGS += -g3 -O0 -rdynamic
debug: LDFLAGS += -Wl,--export-dynamic
debug: ${TARGET}

${TARGET}: ${OBJS} 
	${CXX} ${LDFLAGS} -o $@ $^ ${LIBS} 

${OBJS}: %.o: %.cpp %.dep ${GCH}
	${CXX} ${CXXFLAGS} -o $@ -c $< 

${DEPS}: %.dep: %.cpp Makefile 
	${CXX} ${CXXFLAGS} -MM $< > $@ 

${GCH}: %.gch: ${HEADERS} 
	${CXX} ${CXXFLAGS} -o $@ -c ${@:.gch=.h}

install:
	mkdir -p ${DESTDIR}/${PREFIX}
	cp ${TARGET} ${DESTDIR}/${PREFIX}

uninstall:
	rm ${DESTDIR}/${PREFIX}/${TARGET}

clean:
	rm -f *~ ${DEPS} ${OBJS} ${GCH} ${TARGET} 

distclean: clean



