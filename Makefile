# Variables

VPATH = src
CXXFLAGS = -Wall

# Using implicit rules

all: install jitter_measure_proto

jitter_measure_proto: jitter_measure_proto.c

# place the executable in the binary folder
.PHONY: install
install: jitter_measure_proto
	mkdir bin
	mv $< bin/
