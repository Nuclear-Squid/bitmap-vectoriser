// vim:ft=c
#ifndef _SIMPLIFICATION_H_
#define _SIMPLIFICATION_H_

#include "contours.h"
#include "geometrie2d.h"
#include "linked_lists.h"

double get_distance_from_segment(Point start_segment, Point end_segment, Point point);

// Gives ownership of returned value.
LL_Points* simplification_douglas_peucker(
	const LL_Points* contour,
	const double distance_seuil
);

// Gives ownership of returned value.
LL_Bezier2* simplification_bezier2(
	const LL_Points* contour,
	const double distance_seuil
);

#endif // _SIMPLIFICATION_H_
