#include <stdio.h>  /* utilisation des entrées-sorties standard de C */
#include <stdlib.h> /* utilisation des fonctions malloc et free */

/*---- le type Point et la fonction set_point ----*/
/* ou inclure le module de la tache 2             */
typedef struct { double x,y; } Point;

Point set_point(double x, double y) {
	Point P = {x,y}; return P;
}

/*---- le type cellule de liste de point ----*/
typedef struct Cellule_Liste_Point_ {
	Point data;    /* donnée de l'élément de liste */
	struct Cellule_Liste_Point_* suiv; /* pointeur sur l'élément suivant */
} Cellule_Liste_Point;

/*---- le type liste de point ----*/
typedef struct {
	unsigned int taille;        /* nombre d'éléments dans la liste */
	Cellule_Liste_Point *first; /* pointeur sur le premier élément de la liste */
	Cellule_Liste_Point *last;  /* pointeur sur le dernier élément de la liste */
	                       /* first = last = NULL et taille = 0 <=> liste vide */
} Liste_Point;

typedef Liste_Point Contour; /* type Contour = type Liste_Point */

/*---- le type tableau de point ----*/
typedef struct {
	unsigned int taille; /* nombre d'éléments dans le tableau */
	Point *tab;          /* (pointeur vers) le tableau des éléments */
} Tableau_Point;


/* créer une cellule de liste avec l'élément v 
   renvoie le pointeur sur la cellule de liste créée
   la fonction s'arrete si la création n'a pas pu se faire */
Cellule_Liste_Point *creer_element_liste_Point(Point v) {
	Cellule_Liste_Point *el;
	el = (Cellule_Liste_Point *)malloc(sizeof(Cellule_Liste_Point));
	if (el==NULL)
	{
		fprintf(stderr, "creer_element_liste_Point : allocation impossible\n");
		exit(-1);
	}
	el->data = v;
	el->suiv = NULL;
	return el;
}
/* créer une liste vide */
Liste_Point creer_liste_Point_vide()
{
	Liste_Point L = {0, NULL, NULL};
	return L;
}

/* ajouter l'élément e en fin de la liste L, renvoie la liste L modifiée */
Liste_Point ajouter_element_liste_Point(Liste_Point L, Point e)
{
	Cellule_Liste_Point *el;
	
	el = creer_element_liste_Point(e);
	if (L.taille == 0)
	{
		/* premier élément de la liste */
		L.first = L.last = el;
	}
	else
	{
		L.last->suiv = el;
		L.last = el;
	}
	L.taille++;
	return L;
}

/* suppression de tous les éléments de la liste, renvoie la liste L vide */
Liste_Point supprimer_liste_Point(Liste_Point L)
{
	Cellule_Liste_Point *el=L.first;
	
	while (el) 
	{		
		Cellule_Liste_Point *suiv=el->suiv;
		free(el);
		el = suiv;
	}
	L.first = L.last = NULL; L.taille = 0;
	return L;
}

/* concatène L2 à la suite de L1, renvoie la liste L1 modifiée */
Liste_Point concatener_liste_Point(Liste_Point L1, Liste_Point L2)
{
	/* cas où l'une des deux listes est vide */
	if (L1.taille == 0) return L2; 
	if (L2.taille == 0) return L1;
	
	/* les deux listes sont non vides */
	L1.last->suiv = L2.first; /* lien entre L1.last et L2.first */
	L1.last = L2.last;        /* le dernier élément de L1 est celui de L2 */
	L1.taille += L2.taille;   /* nouvelle taille pour L1 */
	return L1;
}
/* créer une séquence de points sous forme d'un tableau de points 
   à partir de la liste de points L */
Tableau_Point sequence_points_liste_vers_tableau(Liste_Point L)
{
	Tableau_Point T;
	
	/* taille de T = taille de L */
	T.taille = L.taille;
	
	/* allocation dynamique du tableau de Point */
	T.tab = malloc(sizeof(Point) * T.taille);
	if (T.tab == NULL)
	{
		/* allocation impossible : arret du programme avec un message */
		fprintf(stderr, "sequence_points_liste_vers_tableau : ");
		fprintf(stderr, " allocation impossible\n");
		exit(-1);
	}
	
	/* remplir le tableau de points T en parcourant la liste L */
	int k = 0; /* indice de l'élément dans T.tab */
	Cellule_Liste_Point *el = L.first; /* pointeur sur l'élément dans L */
	while (el) 
	{
		T.tab[k] = el->data;
		k++; /* incrémenter k */
		el = el->suiv; /* passer à l'élement suivant dans la liste chainee */
	}
	
	return T;
}

/* écrire le contour L à l'écran 
   cette fonction montre un exemple de conversion d'une liste de points en
   tableau de points afin de pouvoir par la suite accéder aux éléments d'une
   séquence de points par indice */
void ecrire_contour(Liste_Point L)
{
	Tableau_Point TP = sequence_points_liste_vers_tableau(L);
	int k;
	int nP = TP.taille;
	
	printf("%d points : [", nP);
	for (k = 0; k < nP; k++)
	{	
		Point P = TP.tab[k]; /* récupérer le point d'indice k */
		printf(" (%5.1f,%5.1f)", P.x, P.y);
	} 
	printf("]\n");
	
	free(TP.tab); /* supprimer le tableau de point TP */
}

int main()
{
	Contour C1,C2;
	
	/* initialiser C1 comme contour vide */
	C1 = creer_liste_Point_vide();
	printf("C1 : "); ecrire_contour(C1);

	/* ajouter les points (5,3),(3,1),(7,2) et (1,6) dans C1 */
	C1 = ajouter_element_liste_Point(C1, set_point(5,3));
	C1 = ajouter_element_liste_Point(C1, set_point(3,1));
	C1 = ajouter_element_liste_Point(C1, set_point(7,2));
	C1 = ajouter_element_liste_Point(C1, set_point(1,6));
	printf("C1 : "); ecrire_contour(C1);

	/* ajouter le point (4,1) dans C1 */
	C1 = ajouter_element_liste_Point(C1, set_point(4,1));
	printf("C1 : "); ecrire_contour(C1);

	/* créer le contour C2 avec les points (9,5) et (5,7) */
	C2 = creer_liste_Point_vide();	
	C2 = ajouter_element_liste_Point(C2, set_point(9,5));
	C2 = ajouter_element_liste_Point(C2, set_point(5,7));
	printf("C2 : "); ecrire_contour(C2);

	/* concaténer C2 à la suite de C1 */
	C1 = concatener_liste_Point(C1,C2);
	printf("C1 : "); ecrire_contour(C1);

	/* supprimer le contour C1 */
	C1 = supprimer_liste_Point(C1);
	printf("C1 : "); ecrire_contour(C1);
} // fin du programme
