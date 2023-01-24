#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "geometrie2d.h"

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define RESET "\x1b[0m"

// 0 faux tout le reste vrai 
void aff_test(int a){
    if(a){
        printf("%s REUSSI%s\n", GREEN, RESET);
    }else{
        printf("%s NON REUSSI%s\n", RED, RESET);
    }
}

int main(int argc,char** argv){
	if (argc == 1) {
		printf("Faut y donner un fichier en entrée...\n");
		return 1;
	}

    FILE* f = fopen(argv[1], "r");  
    double x1,y1;
    printf("test de la fonction : set_point : \n");

	Point P1,P2;
	//point P1
	fscanf(f, "%lf:%lf\n", &x1, &y1);
	P1=set_point(x1,y1);
	if(P1.x==x1 && P1.y==y1){
		printf("test 1 :");
		aff_test(1);
	}else{
		printf("test 1 :");
		aff_test(0);
	}
	//point P2
	fscanf(f, "%lf:%lf\n", &x1, &y1);
	P2=set_point(x1,y1);
	if(P2.x==x1 && P2.y==y1){
		printf("test 2 :");
		aff_test(1);
	}else{
		printf("test 2 :");
		aff_test(0);
	}

	printf("test de la fonction : set_vec : \n");
	Vecteur V1,V2;
	//V&
	fscanf(f, "%lf:%lf\n", &x1, &y1);
	V1=set_vec(x1,y1);
	if(V1.x==x1 && V1.y==y1){
		printf("test 1 :");
		aff_test(1);
	}else{
		printf("test 1 :");
		aff_test(0);
	}
	//V2
	fscanf(f, "%lf:%lf\n", &x1, &y1);
	V2=set_vec(x1,y1);
	if(V2.x==x1 && V2.y==y1){
		printf("test 2 :");
		aff_test(1);
	}else{
		printf("test 2 :");
		aff_test(0);
	}

	printf("test de la fonction : add_point : \n");
	Point P3;
	P3 = add_point(P1,P2);
	if(P3.x==(P1.x+P2.x) && P3.y==(P1.y+P2.y)){
		printf("test 1 :");
		aff_test(1);
	}else{
		printf("test 1 :");
		aff_test(0);
	}


	printf("test de la fonction : vect_bipoint : \n");
	Vecteur V3;
	V3= vect_bipoint(P1,P2);
	if(V3.x==(P2.x-P1.x) && V3.y==(P2.y-P1.y)){
		printf("test 1 :");
		aff_test(1);
	}else{
		printf("test 1 :");
		aff_test(0);
	}

	printf("test de la fonction : add_vect : \n");
	Vecteur V4;
	V4 = add_vect(V1,V2);
	if(V4.x==(V1.x+V2.x) && V4.y==(V1.y+V2.y)){
		printf("test 1 :");
		aff_test(1);
	}else{
		printf("test 1 :");
		aff_test(0);
	}

	printf("test de la fonction : prod_scal_point : \n");
	Point P4;
	double a = 7;
	P4=prod_scal_point(P1,a);
	if(P4.x == P1.x * a && P4.y == P1.y * a){
		 printf("test 1 :");
		aff_test(1);
	}else{
		printf("test 1 :");
		aff_test(0);
	}

	printf("test de la fonction : prod_scal_vect : \n");
	V4 = prod_scal_vect(V1,7);
	if(V4.x==V1.x*a && V4.y==V1.y*a){
		 printf("test 1 :");
		aff_test(1);
	}else{
		printf("test 1 :");
		aff_test(0);
	}

	printf("test de la fonction : prod_vect : \n");
	double p;
	p=prod_vect(V1,V2);
	if(p==(V1.x*V2.x)+(V1.y*V2.y)){
		 printf("test 1 :");
		aff_test(1);
	}else{
		printf("test 1 :");
		aff_test(0);
	}

	printf("test de la fonction : norme_vect : \n");
	double n;
	n=norme_vect(V2);
	if(n == sqrt(pow(V2.x, 2) + pow(V2.y, 2))){
		 printf("test 1 :");
		aff_test(1);
	}else{
		printf("test 1 :");
		aff_test(0);
	}

	printf("test de la fonction : dis_point : \n");
	double d;
	d= dis_point(P1,P2);
	if(d == sqrt(pow((P1.x - P2.x), 2) + pow((P1.y - P2.y), 2))){
		 printf("test 1 :");
		aff_test(1);
	}else{
		printf("test 1 :");
		aff_test(0);
	}

	fclose(f);
}
