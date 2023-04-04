#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <geometrie2d.h>
#include <contours.h>

//FONCTION NON TESTER 

//On renvoi le point -1,-1 si le i n'est pas dans le tableau 
Point i_element(PointList CONT, int i){
    if(i<=0) return (Point) { -1, -1 };
    ListNode* suivant = CONT->head;
    int j = 1
    while(j<i){
        if( suivant == CONT->tail )&&( j != i ) return (Point) { -1, -1 };
        suivant = suivant->next
        j=j+1;
    }
    return suivant->pos 
}

PointList approx_bezier2(PointList CONT,int j1,int j2){
    float n = j2-j1;
    Point C0 = i_element(CONT,j1);
    Point C2 = i_element(CONT,j2);
    Point C1;
    //calcul de C1
    if(n==1){
        C1= prod_scal_point(add_point( C0, C2), 0,5);
    }else{
        float alpha = (3*n)/((n*n)-1);
        float beta = (1-(2*n))/(2*(n+1)) ;
        for(int i=1; i< n;i++){
            C1= add_point(C1, i_element(CONT,j1+i););
        }
        C1= prod_scal_point(C1, alpha);
        C1= add_point(C1,prod_scal_point(add_point(C0, C2), beta));
    } 

    PointList courbe ;
    ListNode* cb0 = calloc(1,sizeof(ListNode));
    cb0->pos= C0;
    ch0->next = &cb1;
    ListNode* cb1 = calloc(1,sizeof(ListNode));
    cb1->pos= C1;
    ch1->next = &cb2;
    ListNode* cb2 = calloc(1,sizeof(ListNode));
    cb2->pos= C2;
    ch2->next = NULL;

    return courbe ;
}

Point valeur_courbe_2(PointList Courbe ,float t){
    return add_point(add_point(prod_scal_point(Courbe[0],(1-t)*(1-t)),prod_scal_point(Courbe[1],(1-t)*2*t)),prod_scal_point(Courbe[2],t*t));
}

// ici it= i/n
float distance_point_bezier(PointList Courbe,float  ti ,Point P){
    return dis_point(valeur_courbe_2(Courbe,ti), P);
}

PointList simplification_d_p_bezier2(PointList CONST,int j1,int j2,float d){
    float n = j2 -j1;
    char* B = approx_bezier2(CONST ,j1,j2);
    float dmax = 0;
    int k = j1;
    for(int j= j1 + 1; j <= j2,j++){
        float i= j-j1;
        float ti = i/n;
        float dj = distance_point_bezier(B,ti,CONST[j]);
        if(dmax < dj){
            dmax = dj;
            k = j;
        }
    }
    if(dmax <=  d){
        return B;
    }else{
        PointList L1 = simplification_d_p_bezier2(CONST,j1,k,d);
        PointList L2 = simplification_d_p_bezier2(CONST,k,J2,d);

        //concatenation 
        L1->tail->next = L2->head;
        L1->tail = L2->tail 
        return L1
    }
}