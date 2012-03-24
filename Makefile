PKGS=sdl SDL_gfx SDL_image
CFLAGS=-O2 -Wall -W -Itinyscheme `pkg-config --cflags $(PKGS)` -DUSE_INTERFACE=1
LDLIBS=tinyscheme/libtinyscheme.a -lm `pkg-config --libs $(PKGS)`

all: load81 

load81: load81.o editor.o framebuffer.o tinyscheme/libtinyscheme.a
editor.o: editor.c editor.h framebuffer.h
framebuffer.o: framebuffer.c framebuffer.h bitfont.h
load81.o: load81.c framebuffer.h editor.h load81.h

tinyscheme/libtinyscheme.a:
	-(cd tinyscheme && $(MAKE) libtinyscheme.a)

clean:
	rm -f load81 *.o

distclean: clean
	-(cd tinyscheme && $(MAKE) clean)

dep:
	$(CC) -MM *.c
