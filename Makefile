CC = gcc
CPP = g++

CFLAGS = -mx32 -O3 -Wall
CPPFLAGS = -mx32 -O3 -Wall -std=c++11
LFLAGS = 

SRCSC = $(wildcard *.c)

SRCSCPP = $(wildcard *.cpp)

PROGSC = $(patsubst %.c,%,$(SRCSC))

PROGSCPP = $(patsubst %.cpp,%,$(SRCSCPP))

all: $(PROGSC) $(PROGSCPP)

clean:
	rm -rf $(PROGSC) $(PROGSCPP) *.dSYM

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS)

%: %.cpp
	$(CPP) $(CPPFLAGS) -o $@ $< $(LFLAGS)

