CC = gcc
CPP = g++

# Add -mx32 to the flags below, if your compiler supports it, to increase performance.
CFLAGS = -O3 -Wall
CPPFLAGS = -O3 -Wall -std=c++11
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

