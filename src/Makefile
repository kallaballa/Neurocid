TARGET  := libtankwar.so
SRCS    := brain_basic.cpp brain_swarm.cpp brain_nearest.cpp battlefield.cpp canvas.cpp tank.cpp genetic.cpp util.cpp renderer.cpp game.cpp options.cpp gamestate.cpp physics.cpp scanner_bsp.cpp scanner_swarm.cpp scanner_cluster.cpp time_tracker.cpp KMeans.cpp scanner_hybrid.cpp
#precompiled headers
HEADERS := 
CU      := KMeansKernels.cu
CUO     := ${CU:.cu=.o}
GCH    := ${HEADERS:.h=.gch}
OBJS    := ${SRCS:.cpp=.o} 
DEPS    := ${SRCS:.cpp=.dep} 

CXXFLAGS += -fpic -I../ -I/usr/local/cula/include -I/usr/local/cuda-5.0/include/
LDFLAGS += -L/usr/local/cuda/lib64/ -shared -lcuda -lcublas -lcudart
.PHONY: all release debug clean distclean 

all: release
release: ${TARGET}
debug: ${TARGET}
info: ${TARGET}

${CUO}: ${CU} 
	${NVCC} ${NVCC_CXXFLAGS} --compiler-bindir ${NVCC_HOST_CXX} -c $< 

${TARGET}: ${OBJS} ${CUO}
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

