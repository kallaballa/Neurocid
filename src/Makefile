TARGET  := libneurocid.so
SRCS    := brain_swarm.cpp brain_fann.cpp battlefield.cpp canvas.cpp ship.cpp genetic.cpp util.cpp renderer.cpp game.cpp options.cpp gamestate.cpp physics.cpp scanner_bsp.cpp scanner_swarm.cpp scanner_cluster.cpp time_tracker.cpp scanner_hybrid.cpp scan.cpp video_encoder.cpp gui/gui.cpp gui/osd.cpp ../guichan/src/contrib/sdl/sdltruetypefont.cpp background.cpp
ifdef TANKWAR_GPU
SRCS += KMeans.cpp
endif

#precompiled headers
HEADERS := 
CU      := KMeansKernels.cu
CUO     := ${CU:.cu=.o}
GCH    := ${HEADERS:.h=.gch}
OBJS    := ${SRCS:.cpp=.o} 
DEPS    := ${SRCS:.cpp=.dep} 

CXXFLAGS += -fpic -I../ 
LDFLAGS += -shared 
.PHONY: all release debug clean distclean 

ifdef WITH_GPU_KMEANS
CXXFLAGS += -I/usr/local/cula/include/ -I/usr/local/cuda/include/ -D_GPU_KMEANS
LIBS += -lcuda -lcublas -lcudart
LDFLAGS += -L/usr/local/cuda/lib64/
endif

all: release
release: ${TARGET}
debug: ${TARGET}
info: ${TARGET}

ifdef WITH_GPU_KMEANS
${CUO}: ${CU}
	${NVCC} ${NVCC_CXXFLAGS} --compiler-bindir ${NVCC_HOST_CXX} -c $< 
endif

ifdef TANKWAR_GPU
${TARGET}: ${OBJS} ${CUO}
	${CXX} ${LDFLAGS} -o $@ $^ ${LIBS} 
else
${TARGET}: ${OBJS}
	${CXX} ${LDFLAGS} -o $@ $^ ${LIBS}
endif

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
	rm -f *~ ${DEPS} ${OBJS} ${CUO} ${GCH} ${TARGET} 

distclean: clean

