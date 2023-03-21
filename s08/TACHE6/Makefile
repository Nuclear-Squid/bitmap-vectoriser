CC = clang -Wall -Werror

executables = test_contours test_distance_point_vec

all: debug

debug: CC += -g
debug: $(executables)

release: CC += -O3
release: executables


test:
	echo prout

_build:
	[ ! -d _build ] && mkdir _build

_build/image.o: src/image.c src/image.h src/types_macros.h _build 
	$(CC) -c $< -o $@

_build/contours.o: src/contours.c src/image.h src/geometrie2d.h src/types_macros.h _build 
	$(CC) -c $< -o $@

_build/test_contours.o: src/test_contours.c src/image.h src/contours.h
	$(CC) -c $< -o $@

_build/simplifications.o: src/simplifications.c src/simplifications.h src/geometrie2d.h
	$(CC) -c $< -o $@

_build/test_distance_point_vec.o: src/test_distance_point_vec.c _build/simplifications.o
	$(CC) -c $< -o $@

test_contours: _build/contours.o _build/image.o _build/test_contours.o _build/simplifications.o
	$(CC) $^ -o $@ -lm

test_distance_point_vec: _build/simplifications.o _build/test_distance_point_vec.o
	$(CC) $^ -o $@ -lm

clean:
	rm -rf _build $(executables)
