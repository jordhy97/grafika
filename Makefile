CC=g++
CFLAGS=-c -Wall -g -std=c++11
LDFLAGS=-g -lm -pthread

SOURCES=$(wildcard ./src/*.cpp ./src/*/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
MAIN=./src/main.cpp
EXECUTABLE=./bin/main

.PHONY: all bin clean

all: bin

bin: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm $(OBJECTS)
	-rm $(EXECUTABLE)
