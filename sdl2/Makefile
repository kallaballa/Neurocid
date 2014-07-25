TARGET  := neurocid-bin

ifdef JAVASCRIPT
TARGET := neurocid-bin.html
endif

SRCS  := neurocid.cpp eventloop.cpp procedural_scenarios.cpp json_scenario.cpp canvas/SDL2Canvas.cpp
CXXFLAGS += `sdl2-config --cflags` `pkg-config --cflags glew glu gl` -I/usr/include/GL/
LIBS += `sdl2-config --libs` -lSDL2_ttf `pkg-config --libs glew glu gl`

#precompiled headers
HEADERS := 

GCH     := ${HEADERS:.h=.gch}
OBJS    := ${SRCS:.cpp=.o} 
DEPS    := ${SRCS:.cpp=.dep} 


CXXFLAGS += -I../ -I../src/ 
LDFLAGS += -L../src/
LIBS    += -lneurocid 


.PHONY: all release debug clean distclean 

all: release
release: ${TARGET}
debug: ${TARGET}
info: ${TARGET}
profile: ${TARGET}
hardcore: ${TARGET}

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



