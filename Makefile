LDFLAGS = -lSDL2 -lGL -lGLEW
sdl-main: sdl-main.c

.PHONY: clean
clean:
	rm -f sdl-main
