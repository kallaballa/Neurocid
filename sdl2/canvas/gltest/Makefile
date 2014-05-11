CC = gcc

COMPILERFLAGS = -std=c99 -O3
COMPILERFLAGS += -Werror -Wall -Wextra
COMPILERFLAGS += `sdl2-config --cflags` `pkg-config --cflags glew glu gl`
COMPILERFLAGS += -I/usr/local/include/ -I/usr/include/GL/
COMPILERFLAGS += -I/opt/local/include/

# -ggdb -pg

INSTRUMENTFLAGS = -O0 -g3 #-finstrument-functions -lSaturn -O0

LIBPATH = `sdl2-config --libs` -lSDL2_ttf `pkg-config --libs glew glu gl`
LIBPATH += -L/usr/local/lib
LIBPATH += -L/opt/local/lib


OS := $(shell uname)
ifeq ($(findstring Linux,${OS}),Linux)
## Linux: ##
LIBPATH += -lGL -lGLU -lm
else ifeq ($(OS),Darwin)
## OS X: ##
FRAMEWORK = -framework OpenGL
else
## other OS: ##
$(warning Warning: ${OS} probably isn't supported yet ...)
LIBPATH += -lGL -lGLU -lm
endif


All: cellmate

%: %.c
	$(CC) $(COMPILERFLAGS) $(LIBPATH) $(FRAMEWORK) $^ -o $@

instrument: cellmate.c
	$(CC) $(COMPILERFLAGS) $(INSTRUMENTFLAGS) $(LIBPATH) $(FRAMEWORK) $^ -o cellmate

run: cellmate
	./cellmate

cellmate: cellmate.c

clean:
	rm cellmate
