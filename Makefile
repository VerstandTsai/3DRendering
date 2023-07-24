TARGET_OS = Linux

INC = ./include
SRC = ./src
TESTS = ./tests

CXXFLAGS = -Wall -g -pg -std=c++2a -O3 -ffast-math -I$(INC)
LDFLAGS = -L.
LDLIBS = -lprox -lSDL2

ifeq ($(TARGET_OS),Windows)
	SDL_MINGW = ./lib/SDL2-2.28.1/x86_64-w64-mingw32
	CXX = x86_64-w64-mingw32-g++
	CXXFLAGS += -I$(SDL_MINGW)/include
	LDFLAGS += -L$(SDL_MINGW)/lib
	LDLIBS = -lprox -lmingw32 -lSDL2main -lSDL2 -mwindows -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lsetupapi -lversion -luuid -static
else
	CXX = g++
endif

AR = ar
RM = rm

all: libprox.a

%: $(TESTS)/%.cpp libprox.a $(INC)/proxima.h
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)

libprox.a: window.o renderer.o vec3.o objects.o mesh.o texture.o scene.o
	$(AR) -rcs $@ $^

%.o: $(SRC)/%.cpp $(INC)/%.h
	$(CXX) $(CXXFLAGS) -c $<

clean:
	$(RM) -f *.o *.a *.exe

