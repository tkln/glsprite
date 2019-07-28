# Copyright (c) 2019 Aapo Vienamo
# SPDX-License-Identifier: MIT

LDLIBS = -lSDL2 -lGL -lGLEW -lm
CFLAGS = -Wall -g -O2 -I..

OBJS = sdl-main.o glutil.o stb_image.o ../glsprite.o

sdl-main: $(OBJS)

.PHONY: clean
clean:
	rm -f sdl-main $(OBJS)
