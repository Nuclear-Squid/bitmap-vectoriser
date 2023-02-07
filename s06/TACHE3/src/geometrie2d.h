#ifndef _GEOMETRIE2D_H_
#define _GEOMETRIE2D_H_

#include <math.h>

typedef struct {
    double x, y;
} Vecteur;

typedef struct {
    double x, y;
} Point;

/*crée le point de coordonnée*/
static inline Point set_point(double x, double y) {
	return (Point) { x, y };
}

static inline Vecteur set_vec(double x, double y) {
	return (Vecteur) { x, y };
}

/*somme P1+P2*/
static inline Point add_point(Point P1, Point P2) {
	return (Point) { P1.x + P2.x, P1.y + P2.y };
}

/*vecteur correspondant à un bipoint AB*/
static inline Vecteur vect_bipoint(Point A, Point B) {
    return (Vecteur) { B.x - A.x, B.y - A.y };
}

/*somme vecteur */
static inline Vecteur add_vect(Vecteur V1, Vecteur V2) {
	return (Vecteur) { V1.x + V2.x, V1.y + V2.y };
}

/*produit scalaire d'un point ou vecteur avec un chiffre*/
static inline Point prod_scal_point(Point P, double a) {
	return (Point) { P.x * a, P.y * a };
}

static inline Vecteur prod_scal_vect(Vecteur V, double a) {
	return (Vecteur) { V.x * a, V.y * a };
}

/*prodit scalaire entre deux vecteur */
static inline double prod_vect(Vecteur V1, Vecteur V2) {
    return (V1.x * V2.x) + (V1.y * V2.y);
}

/*norme d'un vecteur*/
static inline double norme_vect(Vecteur V) {
    return sqrt(pow(V.x, 2) + pow(V.y, 2));
}

/*Distance entre deux points*/
static inline double dis_point(Point A, Point B) {
    return sqrt(pow((A.x - B.x), 2) + pow((A.y - B.y), 2));
}

#endif
