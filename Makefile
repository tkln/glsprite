LDFLAGS = -lSDL2 -lGL -lGLEW -lm
CFLAGS = -Wall -g -O2

OBJS = sdl-main.o glutil.o stb_image.o glsprite.o

sdl-main: $(OBJS)

.PHONY: clean
clean:
	rm -f sdl-main $(OBJS)
