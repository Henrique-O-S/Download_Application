# Makefile to build the project

# Parameters

CC = gcc
CFLAGS = -Wall

SRC = src/
INCLUDE = include/

HEADERS = $(INCLUDE)/aux.h $(INCLUDE)/macros.h
OBJECTS = main.o aux.o

# Targets

default: download

%.o: %.c $(HEADERS)
	gcc -c $< -o $@

download: $(OBJECTS)
	gcc $(OBJECTS) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f download

