CC = clang -g -lm

all: test_contours

_build:
	[ ! -d _build ] && mkdir _build

_build/image.o: src/image.c src/image.h src/types_macros.h _build 
	$(CC) -c $< -o $@

_build/contours.o: src/contours.c src/image.h src/geometrie2d.h src/types_macros.h _build 
	$(CC) -c $< -o $@

_build/test_contours.o: src/test_contours.c src/image.h src/contours.h
	$(CC) -c $< -o $@

test_contours: _build/contours.o _build/image.o _build/test_contours.o
	$(CC) $^ -o $@

clean:
	rm -rf _build test_contours
