CC = gcc

all: build/main

.PHONY: objects
objects: $(patsubst src/%.c, build/%.o, $(wildcard src/*.c))

build/%.o: src/%.c
	$(CC) -c $< -o $@

build/main: build/main.o build/node.o
	${CC} build/*.o ${LIBS} -o $@
