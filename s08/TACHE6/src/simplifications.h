// vim:ft=c
#ifndef _SIMPLIFICATION_H_
#define _SIMPLIFICATION_H_

#include "contours.h"
#include "geometrie2d.h"

double get_distance_from_segment(Point start_segment, Point end_segment, Point point);

// Gives ownership of returned value.
PointList* simplification_douglas_peucker(
	const PointList* contour,
	const double distance_seuil
);

#endif // _SIMPLIFICATION_H_
