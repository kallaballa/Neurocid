TARGET  := neurocid-bin

ifdef JAVASCRIPT
TARGET := neurocid-bin.html
endif

SRCS  := neurocid.cpp eventloop.cpp procedural_scenarios.cpp json_scenario.cpp sdl/sdlcanvas.cpp ../guichan/src/contrib/sdl/sdltruetypefont.cpp gui/guichangui.cpp gui/help.cpp gui/osd.cpp gui/widgets.cpp theme.cpp
CXXFLAGS += `pkg-config --cflags sdl SDL_image SDL_ttf x11` -I../guichan/include/
LIBS += `pkg-config --libs sdl SDL_image SDL_ttf x11` -lguichan -lguichan_sdl -lpthread
LDFLAGS += -L../guichan/

#precompiled headers
HEADERS := 

GCH     := ${HEADERS:.h=.gch}
OBJS    := ${SRCS:.cpp=.o} 
DEPS    := ${SRCS:.cpp=.dep} 


CXXFLAGS += -I../ -I../src/ 
LDFLAGS += -L../src/
LIBS    += -lneurocid 


ifdef WITHOUT_SDLGFX
CXXFLAGS += -D_NO_SDLGFX
else
CXXFLAGS += `pkg-config --cflags SDL_gfx SDL_image`
LIBS +=  `pkg-config --libs SDL_gfx SDL_image`
endif


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



