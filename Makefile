# Variables

VPATH = src
CFLAGS = -Wall
CXX = gcc

# Using implicit rules


all: install jitter_measure_proto

jitter_measure_proto: jitter_measure_proto.c


# place the executable in the binary folder
.PHONY: install
install: jitter_measure_proto
ifneq ($(wildcard ./bin),)
	mv $< bin/
else
	mkdir bin && mv $< bin/
endif


