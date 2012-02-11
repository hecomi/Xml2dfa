# Makefile for XML2dfa

CXX      = g++-4.6
CXXFLAGS = -std=c++0x
LDFLAGS  = -lmecab -licuio -lboost_regex
SOURCES  = $(shell ls *.cpp)
OBJECTS  = $(SOURCES:.cpp=.o)
OUTPUT   = test

all: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJECTS) -o $(OUTPUT)

clean:
	$(RM) $(OBJECTS) $(OUTPUT)
