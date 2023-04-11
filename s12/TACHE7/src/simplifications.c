#include "simplifications.h"
#include "contours.h"
#include "geometrie2d.h"
#include "linked_lists.h"

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
	}
	else if (projected_pos > 1) {
		return norme_vect(vect_bipoint(end_segment, point));
	}
	else {
		return norme_vect(
			vect_bipoint(point,
				add_point(start_segment, prod_scal_vect(segment, projected_pos))
			)
		);
	}
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

static Bezier2* approx_bezier2(const Point* contour, u_int32_t start, u_int32_t end) {
	const int32_t n = end - start;

	if (n == 1) {
		Point control = prod_scal_point(add_point(contour[start], contour[end]), 0.5);
		return new_bezier_curve2(contour[start], control, contour[end]);
	}

	Point control = { 0, 0 };
	double alpha = (double) (3 * n) / (double) (pow(n, 2) - 1);
	double beta  = (double) (1 - 2 * n) / (double) (2 * (n + 1));

	for (int i = start + 1; i < end; i++) control = add_point(control, contour[i]);

	control = add_point(
			prod_scal_point(control, alpha),
			prod_scal_point(add_point(contour[start], contour[end]), beta)
	);
	return new_bezier_curve2(contour[start], control, contour[end]);
}

static double distance_point_bezier2(Point point, Bezier2* courbe, double pos_courbe) {
	return dis_point(point, evaluate_bezier2(*courbe, pos_courbe));
}

// Ownership of returned value is given to caller
static LL_Bezier2* simplification_b2(
	const Point* contour,
	u_int32_t start,
	u_int32_t end,
	double distance_seuil
) {
	const double step = 1. / (double) (end - start);
	Bezier2* approximation = approx_bezier2(contour, start, end);
	double distance_max = 0;
	u_int32_t indice_distance_max = start;

	for (int i = 1, j = start + 1; j < end; i++, j++) {
		double distance = distance_point_bezier2(contour[j], approximation, i * step);
		if (distance >= distance_max) {
			distance_max = distance;
			indice_distance_max = j;
		}
	}

	if (distance_max <= distance_seuil) {
		LL_Bezier2* rv = new_linked_list(free);
		LL_push_ptr(rv, approximation);
		return rv;
	}
	else {
		free(approximation);
		LL_Bezier2* courbe_gauche = simplification_b2(contour, start, indice_distance_max, distance_seuil);
		LL_Bezier2* courbe_droite = simplification_b2(contour, indice_distance_max, end, distance_seuil);
		LL_concat(courbe_gauche, courbe_droite);
		return courbe_gauche;
	}
}

// Ownership of returned value is given to caller
LL_Bezier2* simplification_bezier2(
	const PointList* contour,
	double distance_seuil
) {
	// Allocate on the heap since it’s likely to be very heavy
	Point* contour_tab = calloc(contour->len, sizeof(Point));
	ListNode* current_node = contour->head;
	for (int i = 0; i < contour->len; i++) {
		contour_tab[i] = current_node->pos;
		current_node = current_node->next;
	}
	LL_Bezier2* rv = simplification_b2(contour_tab, 0, contour->len - 1, distance_seuil);
	free(contour_tab);
	return rv;
}
