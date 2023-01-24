/****************************************************************************** 
  Interface du module image
******************************************************************************/

#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "types_macros.h"

/* 
 Type enuméré Pixel équivalent au char avec BLANC=0 et NOIR=1
 */
typedef enum {BLANC=0,NOIR=1} Pixel;

/* 
 Type Image
 */
typedef struct Image_
{
	UINT la_largeur_de_l_image; 
	UINT la_hauteur_de_l_image; 
	Pixel* pointeur_vers_le_tableau_de_pixels; 
} Image;

/* création d'une image PBM de dimensions L x H avec tous les pixels blancs */
Image creer_image(UINT L, UINT H);

/* suppression de l'image I = *p_I */
void supprimer_image(Image *p_I);

/* renvoie la valeur du pixel (x,y) de l'image I
   si (x,y) est hors de l'image la fonction renvoie BLANC */
Pixel get_pixel_image(Image I, int x, int y);

/* change la valeur du pixel (x,y) de l'image I avec la valeur v
   si (x,y) est hors de l'image la fonction ne fait rien */
void set_pixel_image(Image I, int x, int y, Pixel v);

/* renvoie la largeur de l'image I */
UINT largeur_image(Image I);

/* renvoie la hauteur de l'image I */
UINT hauteur_image(Image I);

/* lire l'image dans le fichier nommé nom_f
   s'il y a une erreur dans le fichier le programme s'arrete en affichant
   un message 
   version acceptant les fichiers avec 
   - ligne 1 : P1
   - zero, une ou plusieurs lignes commençant toutes par #
   - zero, un ou plusieurs séparateurs
   - la largeur
   - un ou plusieurs séparateurs
   - la hauteur
   - un ou plusieurs séparateurs
   - les pixels de l'image
   */
Image lire_fichier_image(char *nom_f);

/* écrire l'image I à l'écran */
void ecrire_image(Image I);

/* calculer l'image "negatif" de l'image I */
/* l'image I n'est pas modifiee et */
/* la fonction renvoie l'image "negatif" de I */
Image negatif_image(Image I);

#endif /* _IMAGE_H_ */
