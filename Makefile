LDFLAGS = -lSDL2 -lGL -lGLEW
CFLAGS = -Wall -g -O2
sdl-main: sdl-main.c

.PHONY: clean
clean:
	rm -f sdl-main
