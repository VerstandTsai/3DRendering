INC = ./includes
SRC = ./src
TESTS = ./tests
CXX = g++
CXXFLAGS = -Wall -g -std=c++2a -O3 -ffast-math -I$(INC)
LDFLAGS = -L.
LDLIBS = -lprox -lSDL2
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
	$(RM) -f *.o *.a

