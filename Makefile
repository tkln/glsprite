LDFLAGS = -lSDL2 -lGL -lGLEW -lm
CFLAGS = -Wall -g -O2
sdl-main: sdl-main.c glutil.c stb_image.c

.PHONY: clean
clean:
	rm -f sdl-main
