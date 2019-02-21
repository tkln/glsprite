LDFLAGS = -lSDL2
sdl-main: sdl-main.c

.PHONY: clean
clean:
	rm -f sdl-main
