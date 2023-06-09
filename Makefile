CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g
INCLUDES = -Isrc

all: build/main

svg: build/main
	mkdir -p build
	./build/main > build/treemap.svg

.PHONY: objects
objects: $(patsubst src/%.c, build/%.o, $(wildcard src/*.c))

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(INCLUDES) -c $(CFLAGS) $< -o $@

build/main: build/main.o build/node.o build/render.o build/util.o
	cp -r static/* build/
	${CC} build/*.o ${LIBS} -o $@

.PHONY: clean
clean:
	rm -rf build
