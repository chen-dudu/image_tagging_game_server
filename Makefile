# # # # # # #
# Makefile for Computer System(COMP30023) project 1
#
# created Mar 2018
# by Matt Farrugia <matt.farrugia@unimelb.edu.au>
# modified April 2019
# by CHEN LIGUO(student ID: 851090)

CC     = gcc
CFLAGS = -Wall -std=c99 -O3
EXE    = image_tagger
OBJ    = main.o handle.o list.o util.o respond.o
#				add any new object files here ^

# top (default) target
all: $(EXE)

# how to link executable
$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)

# other dependencies
main.o: handle.h list.h
handle.o: list.h handle.h util.h respond.h
list.o: handle.h list.h
# ^ add any new dependencies here

# phony targets (do not represent actual files)
.PHONY: clean cleanly all CLEAN
# `make clean` to remove all object files
# `make CLEAN` to remove all object and executable files
# `make cleanall` to `make` then immediately remove object files (inefficient)
clean:
	rm -f $(OBJ)
CLEAN: clean
	rm -f $(EXE)
cleanall: all clean
