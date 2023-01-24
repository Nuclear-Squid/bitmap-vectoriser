CC=clang -g -lm

all: test_geometrie2d
	@echo "oui, y'a un warning, j'ai galèré a intégré math.h."
	@echo "C'est déjà un miracle que ça compile"

test_geometrie2d.o: test_geometrie2d.c geometrie2d.o
	$(CC) -c $< -o $@

geometrie2d.o: geometrie2d.c geometrie2d.h
	$(CC) -c $^

test_geometrie2d: test_geometrie2d.o geometrie2d.o
	$(CC) $^ -o $@
