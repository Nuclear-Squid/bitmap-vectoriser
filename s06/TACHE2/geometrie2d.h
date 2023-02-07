#ifndef _GEOMETRIE2D_H_
#define _GEOMETRIE2D_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    double x, y;  //coordonnées
}Vecteur;

typedef struct {
    double x, y;  //coordonnées
}Point;

/*crée le point de coordonnée*/
Point set_point(double x, double y);

Vecteur set_vec(double x, double y);
/*somme P1+P2*/
Point add_point(Point P1, Point P2);

/*vecteur correspondant à un bipoint AB*/
Vecteur vect_bipoint(Point A, Point B);

/*somme vecteur */
Vecteur add_vect(Vecteur V1, Vecteur V2);

/*produit scalaire d'un point ou vecteur avec un chiffre*/
Point prod_scal_point(Point P, double a);

Vecteur prod_scal_vect(Vecteur V, double a);

/*prodit scalaire entre deux vecteur */
double prod_vect(Vecteur V1, Vecteur V2);

/*norme d'un vecteur*/
double norme_vect(Vecteur V);

/*Distance entre deux points*/
double dis_point(Point A, Point B);

#endif
