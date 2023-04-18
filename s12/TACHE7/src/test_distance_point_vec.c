#include <stdio.h>

#include "simplifications.h"

i32 main(i32 argc, char** argv) {
	if (argc != 7) {
		printf("Usage: %s xA yA xB yB xP yP\n\
avec :\n\
A = Le point de début du segment\n\
B = Le point de fin du segment\n\
P = Le point dont on cherche la distance au segment\n", argv[0]);
		return 0;
	}

	double coords[6] = { 0, };

	for (i32 i = 0; i < 6; i++) {
		if (sscanf(argv[i + 1], "%lf", coords + i) != 1) {
			printf("Argument numéro %d invalide\n", i + 1);
			return 1;
		}
	}

	const Point A = { coords[0], coords[1] };
	const Point B = { coords[2], coords[3] };
	const Point P = { coords[4], coords[5] };

	printf("La distance de P au segment AB est %lf\n", get_distance_from_segment(A, B, P));
	
	return 0;
}
