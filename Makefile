INC = ./includes
SRC = ./src
CXX = g++
CXXFLAGS = -Wall -g -O3 -I$(INC)
LDFLAGS = -L.
LDLIBS = -lprox -lSDL2
AR = ar
RM = rm

all: libprox.a

test: test.cpp libprox.a $(INC)/proxima.h
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)

libprox.a: window.o buffer.o renderer.o vec3.o objects.o
	$(AR) -rcs $@ $^

%.o: $(SRC)/%.cpp $(INC)/%.h
	$(CXX) $(CXXFLAGS) -c $<

clean:
	$(RM) -f *.o *.a

