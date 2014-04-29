TARGET  := libjson_spirit.so
SRCS    := json_spirit_reader.cpp  json_spirit_value.cpp  json_spirit_writer.cpp 
#precompiled headers
HEADERS :=
CU      := 
CUO     := ${CU:.cu=.o}
GCH    := ${HEADERS:.hpp=.gch}
OBJS    := ${SRCS:.cpp=.o}
DEPS    := ${SRCS:.cpp=.dep}

CXX      := g++
CXXFLAGS := -std=c++0x -g0 -O3 -pedantic -Wall -fpic 
LDFLAGS  := -s -L/opt/local/lib -shared
LIBS     := 
.PHONY: all release clean 
DESTDIR := /
PREFIX := /usr/local
MACHINE := $(shell uname -m)

ifeq ($(MACHINE), x86_64)
  LIBDIR = lib64
endif
ifeq ($(MACHINE), i686)
  LIBDIR = lib
endif

all: release
release: ${TARGET}

${TARGET}: ${OBJS}
	${CXX} ${LDFLAGS} -o $@ $^ ${LIBS}

${OBJS}: %.o: %.cpp %.dep ${GCH}
	${CXX} ${CXXFLAGS} -o $@ -c $<

${DEPS}: %.dep: %.cpp Makefile
	${CXX} ${CXXFLAGS} -MM $< > $@

${GCH}: %.gch: ${HEADERS}
	${CXX} ${CXXFLAGS} -o $@ -c ${@:.gch=.hpp}

clean:
	rm -f *~ ${DEPS} ${OBJS} ${CUO} ${GCH} ${TARGET} 

