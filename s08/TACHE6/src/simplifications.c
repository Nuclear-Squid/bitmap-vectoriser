#include "simplifications.h"
#include "contours.h"
#include "geometrie2d.h"

double get_distance_from_segment(Point start_segment, Point end_segment, Point point) {
	Vecteur segment = vect_bipoint(start_segment, end_segment);
	Vecteur start_to_point = vect_bipoint(start_segment, point);

	if (point_equals(start_segment, end_segment)) {
		return norme_vect(start_to_point);
	}

	double projected_pos =
			prod_vect(start_to_point, segment) /
			prod_vect(segment, segment);

	if (projected_pos < 0) {
		return norme_vect(start_to_point);
	} else if (projected_pos > 1) {
		return norme_vect(vect_bipoint(end_segment, point));
	}
	return norme_vect(
		vect_bipoint(point,
			add_point(start_segment, prod_scal_vect(segment, projected_pos))
		)
	);
}

static PointList* simplification(
	const ListNode* start,
	const ListNode* end,
	const double distance_seuil
) {
	const ListNode* current_node = start;
	double distance_max = 0;
	const ListNode* node_max_distance;

	while (current_node != end) {
		double distance =
				get_distance_from_segment(start->pos, end->pos, current_node->pos);
		if (distance > distance_max) {
			distance_max = distance;
			node_max_distance = current_node;
		}
		current_node = current_node->next;
	}

	if (distance_max <= distance_seuil) {
		// When it simplifies down to a point, erase it
		/* if (point_equals(start->pos, end->pos)) NULL; */

		PointList* rv = malloc(sizeof(ListNode));
		*rv = (PointList) {
			.head = malloc(sizeof(ListNode)),
			.tail = malloc(sizeof(ListNode)),
			.len = 2
		};

		*rv->head = (ListNode) { start->pos, rv->tail };
		*rv->tail = (ListNode) { end->pos, NULL };

		return rv;
	}

	PointList* segment_gauche = simplification(start, node_max_distance, distance_seuil);
	PointList* segment_droit  = simplification(node_max_distance, end, distance_seuil);

	segment_gauche->tail->next = segment_droit->head->next;
	segment_gauche->tail = segment_droit->tail;
	segment_gauche->len += segment_droit->len - 1;
	free(segment_droit->head);
	free(segment_droit);

	return segment_gauche;
}

PointList* simplification_douglas_peucker(
	const PointList* contour,
	const double distance_seuil
) {
	return simplification(contour->head, contour->tail, distance_seuil);
}
