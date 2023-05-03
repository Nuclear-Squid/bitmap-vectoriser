#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "types_macros.h"
#include "linked_lists.h"
#include "geometrie2d.h"
#include "simplifications.h"

double drand (double low, double high) {
	return ( (double) rand() * (high - low) ) / (double) RAND_MAX + low;
}

int main() {
	srand(time(NULL));

	{  // Bezier3, n = 1
		for (u32 i = 0; i < 100000; i++) {
			const Point P0 = { drand(-100, 100), drand(-100, 100) };
			const Point P1 = { drand(-100, 100), drand(-100, 100) };
			const Point* contour[] = { &P0, &P1 };

			Bezier3* curve = approx_bezier3(contour, 0, 1);

			assert(point_equals(P0, curve->start));
			assert(point_equals(
				curve->control1,
				prod_scal_point(add_point(P1, prod_scal_point(P0, 2)), 1. / 3.)
			));
			assert(point_equals(
				curve->control2,
				prod_scal_point(add_point(P0, prod_scal_point(P1, 2)), 1. / 3.)
			));
			assert(point_equals(P1, curve->end));

			free(curve);
		}

		printf("Bezier3, n = 1 : Tests réussis\n");
	}

	{  // Bezier3, n = 2
		for (u32 i = 0; i < 100000; i++) {
			const Point P0 = { drand(-100, 100), drand(-100, 100) };
			const Point P1 = { drand(-100, 100), drand(-100, 100) };
			const Point P2 = { drand(-100, 100), drand(-100, 100) };
			const Point* contour[] = { &P0, &P1, &P2 };

			Bezier3* curve = approx_bezier3(contour, 0, 2);

			assert(point_equals(P0, curve->start));
			assert(point_equals(
				curve->control1,
				prod_scal_point(vect_bipoint(P2, prod_scal_point(P1, 4)), 1. / 3.)
			));
			assert(point_equals(
				curve->control2,
				prod_scal_point(vect_bipoint(P0, prod_scal_point(P1, 4)), 1. / 3.)
			));
			assert(point_equals(P2, curve->end));

			free(curve);
		}

		printf("Bezier3, n = 2 : Tests réussis\n");
	}

	{  // Bezier3, n > 2
		for (u32 i = 0; i < 100000; i++) {
			const Bezier3 original_curve = {
				{ drand(-100, 100), drand(-100, 100) },
				{ drand(-100, 100), drand(-100, 100) },
				{ drand(-100, 100), drand(-100, 100) },
				{ drand(-100, 100), drand(-100, 100) }
			};

			/* const u32 n = rand() % 50 + 4; */
			const u32 n = 50000;
			const double step = 1. / (double) n;
			Point** contour = malloc((n + 1) * sizeof(Point*));
			for (u32 j = 0; j < n + 1; j++) {
				contour[j] = malloc(sizeof(Point));
				*contour[j] = evaluate_bezier3(original_curve, j * step);
			}

			// **VERY** unsafe cast, use with caution
			Bezier3* curve = approx_bezier3((const Point**) contour, 0, n);

			printf("(%lf, %lf) == (%lf, %lf)\n",
					original_curve.control1.x, original_curve.control1.y,
					curve->control1.x, curve->control1.y);

			assert(point_equals(original_curve.start, curve->start));
			assert(point_equals(original_curve.control1, curve->control1));
			assert(point_equals(original_curve.control2, curve->control2));
			assert(point_equals(original_curve.end, curve->end));

			for (u32 j = 0; j < n; j++) {
				free(contour[j]);
			}
			free(contour);
			free(curve);
		}

		printf("Bezier3, n >= 3 : Tests réussis\n");
	}

	return 0;
}
