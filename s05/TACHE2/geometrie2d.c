#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "geometrie2d.h"

/*crée le point de coordonnée*/
Point set_point(double x,double y){
    Point P ={x,y};
    return P;
}

Vecteur set_vec(double x,double y){
    Vecteur V ={x,y};
    return V;
}
/*somme P1+P2*/
Point add_point(Point P1 , Point P2){
    return set_point(P1.x + P2.x , P1.y + P2.y);
}

/*vecteur correspondant à un bipoint AB*/
Vecteur vect_bipoint(Point A,Point B){
    Vecteur V = {B.x-A.x,B.y-A.y};
    return V;
}

/*somme vecteur */
Vecteur add_vect(Vecteur V1,Vecteur V2){
    Vecteur V = {V1.x + V2.x , V1.y + V2.y};
    return V;
}

/*produit scalaire d'un point ou vecteur */
Point prod_scal_point(Point P,double a){
    Point R ={P.x*a,P.y*a};
    return R;
}

Vecteur prod_scal_vect(Vecteur V,double a){
    Vecteur R ={V.x*a,V.y*a};
    return R;
}

/*prodit scalaire entre deux vecteur */
double prod_vect(Vecteur V1,Vecteur V2){
    double p;
    p = (V1.x*V2.x)+(V1.y*V2.y);
    return p;
}

/*norme d'un vecteur*/
double norme_vect(Vecteur V) {
    return sqrt(pow(V.x, 2) + pow(V.y, 2));
}

/*Distance entre deux points*/
double dis_point(Point A,Point B){
    return sqrt(pow((A.x - B.x), 2) + pow((A.y - B.y), 2));
}
