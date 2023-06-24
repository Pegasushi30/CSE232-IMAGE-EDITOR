SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:src/%.c=lib/%.o)

all: lib/gfx.o lib/bmp.o lib/IEdit.o 
	gcc lib/gfx.o lib/bmp.o lib/IEdit.o -o IEdit -lX11 -lm

lib/%.o: src/%.c
	gcc -c $< -o $@ -Iinclude
