TARGET_OS = Linux

BIN = ./bin
INC = ./include
SRC = ./src
LIB = ./lib
TESTS = ./tests

CXXFLAGS = -Wall -g -pg -std=c++2a -O3 -ffast-math -I$(INC) -I$(LIB)/stb_image/include
LDFLAGS = -L. -L$(LIB)/stb_image/lib
LDLIBS = -lprox -lstb_image -lSDL2

ifeq ($(TARGET_OS),Windows)
	SDL_MINGW = $(LIB)/SDL2-mingw32
	DLLS = /usr/lib/gcc/x86_64-w64-mingw32/9.3-win32
	CXX = x86_64-w64-mingw32-g++
	CXXFLAGS += -I$(SDL_MINGW)/include
	LDFLAGS += -L$(SDL_MINGW)/lib
	LDLIBS = -lprox -lstb_image-mingw32 -lmingw32 -lSDL2main -lSDL2 -mwindows
else
	CXX = g++
endif

AR = ar
RM = rm

all: libprox.a

%: $(TESTS)/%.cpp libprox.a $(INC)/proxima.h
	$(CXX) $(CXXFLAGS) -o $(BIN)/$@ $< $(LDFLAGS) $(LDLIBS)
	@if [ $(TARGET_OS) = "Windows" ];\
	then\
		cp $(SDL_MINGW)/bin/SDL2.dll ./bin;\
		cp $(DLLS)/libstdc++-6.dll ./bin;\
		cp $(DLLS)/libgcc_s_seh-1.dll ./bin;\
		cp -r ./assets ./bin;\
	fi

libprox.a: window.o renderer.o vec3.o objects.o mesh.o texture.o scene.o
	$(AR) -rcs $@ $^

%.o: $(SRC)/%.cpp $(INC)/%.h
	$(CXX) $(CXXFLAGS) -c $<

clean:
	$(RM) -rf $(BIN)/* *.o *.a

