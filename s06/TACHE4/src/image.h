/****************************************************************************** 
  Interface du module image
******************************************************************************/

#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <stdbool.h>
#include "types_macros.h"

typedef bool Pixel;

#define NOIR  true
#define BLANC false

typedef struct {
	UINT largeur;
	UINT hauteur; 
	Pixel* pixels; 
} Image;

/* macro donnant l'indice du pixel de coordonnees (_x,_y) de l'image _I 
   dans le tableau de pixels de l'image _I */
#define INDICE_PIXEL(_I,_x,_y) ((_x)-1)+(_I).largeur*((_y)-1)

/* renvoie la valeur du pixel (x,y) de l'image I
   si (x,y) est hors de l'image la fonction renvoie BLANC */
static inline Pixel get_pixel_image(Image I, int x, int y) {
	if (x < 1 || x > I.largeur || y < 1 || y > I.hauteur)
		return BLANC;
	return I.pixels[INDICE_PIXEL(I, x, y)];
}

/* change la valeur du pixel (x,y) de l'image I avec la valeur v
   si (x,y) est hors de l'image la fonction ne fait rien */
static inline void set_pixel_image(Image I, int x, int y, Pixel v) {
	if (x < 1 || x > I.largeur || y < 1 || y > I.hauteur)
		return;
	I.pixels[INDICE_PIXEL(I, x, y)] = v;
}

/* creation d'une image PBM de dimensions L x H avec tous les pixels blancs */
static inline Image creer_image(UINT L, UINT H) {
	return (Image) {
		.largeur = L,
		.hauteur = H,
		.pixels = (Pixel*) calloc(L * H, sizeof(Pixel)),
	};
}

/* suppression de l'image I = *p_I */
static inline void supprimer_image(Image* p_image) {
	free(p_image->pixels);
	*p_image = (Image) { 0, 0, NULL };
}

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

// Tourne l'image de 90 degrés sur la droite
Image rotation_90_deg_image(Image I);
	
// Tourne l'image de `angle` degrés dans le sens trigonométrique
Image rotation_image(Image I, double angle);

#endif /* _IMAGE_H_ */
