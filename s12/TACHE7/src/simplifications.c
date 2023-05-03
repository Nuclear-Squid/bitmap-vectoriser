#include <assert.h>

#include "simplifications.h"
#include "contours.h"
#include "geometrie2d.h"
#include "linked_lists.h"

// ====<<+-------------------------+>>====
// ====<<| Simplification Segments |>>====
// ====<<+-------------------------+>>====

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

static LL_Points* simplification(
	const LinkedListNode* start,
	const LinkedListNode* end,
	const double distance_seuil
) {
	double distance_max = 0;
	const LinkedListNode* node_max_distance;
	const Point* start_point = start->content;
	const Point* end_point = end->content;

	for (const LinkedListNode* current_node = start->next;
			current_node != end; LN_next(current_node))
	{
		const Point* current_point = current_node->content;
		double distance = get_distance_from_segment(*start_point, *end_point, *current_point);
		if (distance >= distance_max) {
			distance_max = distance;
			node_max_distance = current_node;
		}
	}

	if (distance_max <= distance_seuil) {
		return LL_from_ptr(free, 2,
			new_point(start_point->x, start_point->y),
			new_point(end_point->x, end_point->y));
	}
	else {
		LL_Points* segment_gauche = simplification(start, node_max_distance, distance_seuil);
		LL_Points* segment_droit  = simplification(node_max_distance, end, distance_seuil);

		segment_droit->dealloc(LL_pop_ptr(segment_droit));
		LL_concat(segment_gauche, segment_droit);
		return segment_gauche;
	}
}

LL_Points* simplification_douglas_peucker(
	const LL_Points* contour,
	double distance_seuil
) {
	return simplification(contour->head, contour->tail, distance_seuil);
}

// ====<<+-------------------------+>>====
// ====<<| Simplification Bézier 2 |>>====
// ====<<+-------------------------+>>====

i64 ipow(i64 val, u64 exp) {
	i64 rv = val;
	for (u64 i = 0; i < exp - 1; i++) rv *= val;
	return rv;
}

static double distance_point_bezier2(Point point, Bezier2* courbe, double pos_courbe) {
	return dis_point(point, evaluate_bezier2(*courbe, pos_courbe));
}

static Bezier2* approx_bezier2(const Point** contour, u32 start, u32 end) {
	const i32 ecart_noeuds = end - start;

	if (ecart_noeuds == 1) {
		Point control = prod_scal_point(add_point(*contour[start], *contour[end]), 0.5);
		return new_bezier_curve2(*contour[start], control, *contour[end]);
	}

	Point control = { 0, 0 };
	double alpha = (double) (3 * ecart_noeuds) / (double) (ipow(ecart_noeuds, 2) - 1);
	double beta  = (double) (1 - 2 * ecart_noeuds) / (double) (2 * (ecart_noeuds + 1));

	for (u32 i = start + 1; i < end; i++) control = add_point(control, *contour[i]);

	control = add_point(
			prod_scal_point(control, alpha),
			prod_scal_point(add_point(*contour[start], *contour[end]), beta)
	);
	return new_bezier_curve2(*contour[start], control, *contour[end]);
}

// Ownership of returned value is given to caller
static LL_Bezier2* simplification_b2(
	const Point** contour,
	u32 start,
	u32 end,
	double distance_seuil
) {
	const double step = 1. / (double) (end - start);
	Bezier2* approximation = approx_bezier2(contour, start, end);
	double distance_max = 0;
	u32 indice_distance_max = start;

	for (u32 i = 1, j = start + 1; j < end; i++, j++) {
		const double distance = distance_point_bezier2(*contour[j], approximation, i * step);
		if (distance >= distance_max) {
			distance_max = distance;
			indice_distance_max = j;
		}
	}

	if (distance_max <= distance_seuil) {
		return LL_from_ptr(free, 1, approximation);
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
LL_Bezier2* simplification_bezier2(const LL_Points* contour, double distance_seuil) {
	// Allocate on the heap since it’s likely to be very heavy
	const Point** contour_tab = calloc(contour->len, sizeof(Point*));
	Point* current_point;
	i32 i = 0;
	LL_for_each_ptr(contour, current_point) contour_tab[i++] = current_point;

	LL_Bezier2* rv = simplification_b2(contour_tab, 0, contour->len - 1, distance_seuil);
	free(contour_tab);
	return rv;
}

// ====<<+-------------------------+>>====
// ====<<| Simplification Bézier 3 |>>====
// ====<<+-------------------------+>>====

static double distance_point_bezier3(Point point, Bezier3* courbe, double pos_courbe) {
	return dis_point(point, evaluate_bezier3(*courbe, pos_courbe));
}

static double coeff_point(i32 ecart_noeuds, i32 pos_noeud) {
	return (double) ( (6 * ipow(pos_noeud, 4)) - (8 * ecart_noeuds * ipow(pos_noeud, 3)) +
			(6 * ipow(pos_noeud, 2)) - (4 * ecart_noeuds * pos_noeud) +
			ipow(ecart_noeuds, 4) - ipow(ecart_noeuds, 2) );
}

Bezier3* approx_bezier3(const Point** contour, u32 start, u32 end) {
	const i64 ecart = end - start;

	if (ecart < 3) {
		Bezier2* approximation = approx_bezier2(contour, start, end);
		Bezier3* rv = malloc(sizeof(Bezier3));
		*rv = bezier3_from_bezier2(*approximation);
		free(approximation);
		return rv;
	}

	/* printf("%ld\n", 3 * (ipow(ecart, 2) - 1) * (ipow(ecart, 2) - 4) * ((3 * ipow(ecart, 2)) + 1)); */

	const double alpha =
			(double) ((-15 * ipow(ecart, 3)) + (5 * ipow(ecart, 2)) + (2 * ecart) + 4) /
			(double) (3 * (ecart + 2) * ((3 * ipow(ecart, 2)) + 1));
	const double beta =
			(double) ((10 * ipow(ecart, 3)) - (15 * ipow(ecart, 2)) + ecart + 2) /
			(double) (3 * (ecart + 2) * ((3 * ipow(ecart, 2)) + 1));
	const double lambda = (double) (70 * ecart) /
			(double) (3 * (pow(ecart, 2) - 1) * (pow(ecart, 2) - 4) * ((3 * pow(ecart, 2)) + 1));

	/* printf("alpha : %lf | beta : %lf | lambda : %lf\n", alpha, beta, lambda); */

	Point control1 = { 0, 0 };
	Point control2 = { 0, 0 };
	for (u32 i = 1, j = start + 1; i < (u32) ecart; i++, j++) {
		/* printf("%lf %lf\n", coeff_point(ecart, i), coeff_point(ecart, ecart - i)); */
		/* printf("(%lf, %lf)\n", control1.x, control1.y); */
		control1 = add_point(control1,
				prod_scal_point(*contour[j], coeff_point(ecart, i)));
		control2 = add_point(control2,
				prod_scal_point(*contour[j], coeff_point(ecart, ecart - i)));
	}

	control1 = add_point(
			prod_scal_point(control1, lambda),
			add_point(
				prod_scal_point(*contour[start], alpha),
				prod_scal_point(*contour[end], beta)));
	control2 = add_point(
			prod_scal_point(control2, lambda),
			add_point(
				prod_scal_point(*contour[start], beta),
				prod_scal_point(*contour[end], alpha)));
	return new_bezier_curve3(*contour[start], control1, control2, *contour[end]);
}

// Ownership of returned value is given to caller
static LL_Bezier3* simplification_b3(
	const Point** contour,
	u32 start,
	u32 end,
	double distance_seuil
) {
	const double step = 1. / (double) (end - start);
	Bezier3* approximation = approx_bezier3(contour, start, end);
	double distance_max = 0;
	u32 indice_distance_max = start;

	for (u32 i = 1, j = start + 1; j < end; i++, j++) {
		const double distance = distance_point_bezier3(*contour[j], approximation, i * step);
		if (distance >= distance_max) {
			distance_max = distance;
			indice_distance_max = j;
		}
	}

	if (distance_max <= distance_seuil) {
		return LL_from_ptr(free, 1, approximation);
	}
	else {
		free(approximation);
		LL_Bezier3* courbe_gauche = simplification_b3(contour, start, indice_distance_max, distance_seuil);
		LL_Bezier3* courbe_droite = simplification_b3(contour, indice_distance_max, end, distance_seuil);
		LL_concat(courbe_gauche, courbe_droite);
		return courbe_gauche;
	}
}

// Ownership of returned value is given to caller
LL_Bezier3* simplification_bezier3(const LL_Points* contour, double distance_seuil) {
	// Allocate on the heap since it’s likely to be very heavy
	const Point** contour_tab = calloc(contour->len, sizeof(Point*));
	Point* current_point;
	i32 i = 0;
	LL_for_each_ptr(contour, current_point) contour_tab[i++] = current_point;

	LL_Bezier3* rv = simplification_b3(contour_tab, 0, contour->len - 1, distance_seuil);
	free(contour_tab);
	return rv;
}

