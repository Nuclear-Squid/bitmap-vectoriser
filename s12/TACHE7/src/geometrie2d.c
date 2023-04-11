#include <stdlib.h>

#include "geometrie2d.h"

Bezier2* new_bezier_curve2(Point start, Point control, Point end) {
	Bezier2* rv = calloc(1, sizeof(Bezier2));
	*rv = (Bezier2) { start, control, end };
	return rv;
}

Point evaluate_bezier2(Bezier2 courbe, double x) {
	double a = pow((1. - x), 2.);
	double b = 2. * x * (1. - x);
	double c = pow(x, 2.);
	return (Point) {
		(a * courbe.start.x) + (b * courbe.control.x) + (c * courbe.end.x),
		(a * courbe.start.y) + (b * courbe.control.y) + (c * courbe.end.y),
	};
}

Bezier3* new_bezier_curve3(Point start, Point control1, Point control2, Point end) {
	Bezier3* rv = calloc(1, sizeof(Bezier3));
	*rv = (Bezier3) { start, control1, control2, end };
	return rv;
}

Point evaluate_bezier3(Bezier3 courbe, double x) {
	double a = pow((1.0 - x), 3);
	double b = pow((1.0 - x), 2) * x;
	double c = pow(x, 2) * (1.0 - x);
	double d = pow(x, 3);
	return (Point) {
		(a * courbe.start.x) + (b * courbe.control1.x) +
				(c * courbe.control2.x) + (d * courbe.end.x),
		(a * courbe.start.y) + (b * courbe.control1.y) +
				(c * courbe.control2.x) + (d * courbe.end.y),
	};
}

Bezier3 bezier3_from_bezier2(Bezier2 input) {
	return (Bezier3) {
		.start = input.start,
		.end   = input.end,
		.control1 = {
			(2.0 * input.start.x + input.control.x) / 3,
			(2.0 * input.start.y + input.control.y) / 3,
		},
		.control2 = {
			(2.0 * input.end.x + input.control.x) / 3,
			(2.0 * input.end.y + input.control.y) / 3,
		},
	};
}
