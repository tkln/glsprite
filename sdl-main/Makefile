# Copyright (c) 2019 Aapo Vienamo
# SPDX-License-Identifier: MIT

LDLIBS = -lSDL2 -lGL -lGLEW -lm
CFLAGS = -Wall -g -O2 -I.. -I../vecmat/include/
CXXFLAGS = $(CFLAGS)

OBJS = glutil.o stb_image.o ../glsprite.o

.PHONY: default
default: sdl-main sdl-mainpp

sdl-main: sdl-main.o $(OBJS)
sdl-mainpp: sdl-mainpp.o $(OBJS)

.PHONY: clean
clean:
	rm -f sdl-main sdl-mainpp sdl-main.o sdl-mainpp.o $(OBJS)
