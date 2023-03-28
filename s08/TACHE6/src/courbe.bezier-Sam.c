#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <geometrie2d.h>

// objecctif :
// - mettre tout les char* en liste chainer (=PointList 


//CONT un tableau de coordonnées de point (NON TESTER)
char* approx_bezier(char* CONT,int j1,int j2){
    float n = j2-j1;
    Point C0 = CONT[j1];
    Point C2 = CONT[j2];
    Point C1;
    //calcul de C1
    if(n==1){
        C1= prod_scal_point(add_point(CONT[j1], CONT[j2]), 0,5);
    }else{
        float alpha = (3*n)/((n*n)-1);
        float beta = (1-(2*n))/(2*(n+1)) ;
        for(int i=1; i< n;i++){
            C1= add_point(C1, CONT[j1+i]);
        }
        C1= prod_scal_point(C1, alpha);
        C1= add_point(C1,prod_scal_point(add_point(CONT[j1], CONT[j2]), beta));
    }
    char* Courbe = malloc(sizeof(Point));
    Courbe[0]=C0;
    Courbe[1]=C1;
    Courbe[2]=C2;
    return Courbe ;
}

Point valeur_courbe_2(char* Courbe ,float t){
    return add_point(add_point(prod_scal_point(Courbe[0],(1-t)*(1-t)),prod_scal_point(Courbe[1],(1-t)*2*t)),prod_scal_point(Courbe[2],t*t));
}

// ici it= i/n
float distance_point_bezier(char* Courbe,float ,Point P){
    return dis_point(valeur_courbe_2(Courbe,it), P);
}

void simplification_d_p_bezier2(char *CONST,int j1,int j2,float d){
    float n = j2 -j1;

}