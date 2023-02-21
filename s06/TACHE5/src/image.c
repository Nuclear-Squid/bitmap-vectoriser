/****************************************************************************** 
  Implementation du module image_pbm
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "types_macros.h"
#include "image.h"

/* lire l'image dans le fichier nomme nom_f
   s'il y a une erreur dans le fichier le programme s'arrete en affichant
   un message 
   version acceptant les fichiers avec 
   - ligne 1 : P1
   - zero, une ou plusieurs lignes commençant toutes par #
   - zero, un ou plusieurs separateurs
   - la largeur
   - un ou plusieurs separateurs
   - la hauteur
   - un ou plusieurs separateurs
   - les pixels de l'image
   */

/* teste si le fichier d'identificateur f debute par un en-tete
   valide pour un fichier PBM :
   - ligne 1 : P1
   - suivie de zero, une ou plusieurs lignes commençant toutes par #
   La fonction se termine correctement si le fichier est correct, 
   et le pointeur de fichier se trouve à la suite de l'entete.
   Sinon, l'execution du programme est arretee avec l'affichage d'un message
   d'erreur (appel à ERREUR_FATALE)
    */ 
static void entete_fichier_pbm(FILE *f)
{
	char *ligne;
	size_t n;
	ssize_t l_ligne;

	/* se positionner en debut de fichier */
	fseek(f, 0, SEEK_SET);

	/* lecture et test de la ligne 1 */
	ligne = (char *)NULL;
	n = 0;
	l_ligne = getline(&ligne, &n, f);

	/* la ligne est correcte si et ssi 
	   cas - fichier cree sous Linux : ligne = {'P','1',10} 
	     soit une chaine de 3 caracteres (le dernier est le caractere nul) 
	   cas - fichier cree sous Windows : ligne = {'P','1',13, 10} 
	     soit une chaine de 4 caracteres (le dernier est le caractere nul) 
	   */
	if (l_ligne < 3)
	{
		ERREUR_FATALE("entete_fichier_pbm : ligne 1 incorrecte\n");
	}
	if ((ligne[0] != 'P') || (ligne[1] != '1'))
	{
		ERREUR_FATALE("entete_fichier_pbm : ligne 1 incorrecte\n");
	}
	free(ligne);

	/* lecture des eventuelles lignes commençant par # */
	bool boucle_ligne_commentaire = true;
	do
	{
		/* tester d'abord la fin de fichier */
		if (feof(f))
		{
			ERREUR_FATALE("entete_fichier_pbm : fin fichier inattendue\n");
		}
		
		/* lire un caractere et tester par rapport à '#' */
		char c;
		fscanf(f, "%c", &c);
		if (c=='#')
		{
			/* lire le reste de la ligne */
			ligne = (char *)NULL;
			n = 0;
			l_ligne = getline(&ligne, &n, f);
			free(ligne);
		}
		else
		{
			/* reculer d'un caractère dans f */
			fseek(f, -1, SEEK_CUR);
			boucle_ligne_commentaire = false;
		}
	} while (boucle_ligne_commentaire);

}

/* lire l'image dans le fichier nomme nom_f
   s'il y a une erreur dans le fichier le programme s'arrete en affichant
   un message */
Image lire_fichier_image(char *nom_f)
{
	FILE *f;
	UINT L,H;
	UINT x,y;
	int res_fscanf;
	Image I;

	/* ouverture du fichier nom_f en lecture */
	f = fopen(nom_f, "r");
	if (f == (FILE *)NULL)
	{
		ERREUR_FATALE("lire_fichier_image : ouverture du fichier impossible\n");
	}

	/* traitement de l'en-tete et arret en cas d'erreur */
	entete_fichier_pbm(f);

	/* lecture des dimensions */
	res_fscanf = fscanf(f, "%d", &L);
	if (res_fscanf != 1)
	{
		ERREUR_FATALE("lire_fichier_image : dimension L incorrecte\n");
	}
	res_fscanf = fscanf(f, "%d", &H);
	if (res_fscanf != 1)
	{
		ERREUR_FATALE("lire_fichier_image : dimension H incorrecte\n");
	}

	/* creation de l'image de dimensions L x H */
	I = creer_image(L,H);

	/* lecture des pixels du fichier 
	   seuls les caracteres '0' (BLANC) ou '1' (NOIR) 
	   doivent etre pris en compte */
	x = 1; y = 1;
	while (!feof(f) && y<=H)
	{
		char c;
		int res;
		
		/* lire un caractere en passant les caracteres differents de '0' et '1' */
		res = fscanf(f, "%c", &c);
		while (!feof(f) && !(c == '0' || c == '1'))
		{
			res = fscanf(f, "%c", &c);
		}
		if (!feof(f))
		{
			set_pixel_image(I,x,y,c=='1' ? NOIR : BLANC );
			x++;
			if (x>L)
			{
				x = 1; y++;
			}
		}
	}   

	/* fermeture du fichier */
	fclose(f);

	return I;
}

/* ecrire l'image I à l'ecran */
void ecrire_image(Image I)
{
	UINT largeur = I.largeur;
	UINT hauteur = I.hauteur;

	for (int u=1; u <= largeur + 2; u++) {
		printf("\e[100m  \e[0m");
	}
	printf("\n");

	for (int i=1; i <= hauteur; i++) {
		printf("\e[100m  \e[0m");
		for (int u=1; u <= largeur; u++) {
			/* printf("%c", get_pixel_image(I, u, i) == BLANC ? ' ' : 'X'); */
			if (get_pixel_image(I, u, i) == NOIR)
				printf ("\e[7m");
			printf("  \e[0m");
		}
		printf("\e[100m  \e[0m\n");
	}

	for (int u=1; u <= largeur + 2; u++) {
		printf("\e[100m  \e[0m");
	}
	printf("\n\n");
}

/* calculer l'image "negatif" de l'image I */
/* l'image I n'est pas modifiee et */
/* la fonction renvoie l'image "negatif" de I */
Image negatif_image(Image I)
{
	UINT largeur = I.largeur;
	UINT hauteur = I.hauteur;

	Image rv = creer_image(largeur, hauteur);

	for (int i=1; i <= hauteur; i++) {
		for (int u=1; u <= largeur; u++) {
			Pixel nouveau_pix = get_pixel_image(I, u, i) == BLANC ? NOIR : BLANC;
			set_pixel_image(rv, u, i, nouveau_pix);
		}
	}

	return rv;
}

// Tourne l'image de 90 degrés sur la droite
Image rotation_90_deg_image(Image I)
{
	UINT largeur = I.largeur;
	UINT hauteur = I.hauteur;

	// Échange les dimensions pour tourner l'image
	Image rv = creer_image(hauteur, largeur);

	for (int i=1; i <= hauteur; i++) {
		for (int u=1; u <= largeur; u++) {
			set_pixel_image(rv, hauteur - i + 1, u, get_pixel_image(I, u, i));
		}
	}
	
	return rv;
}


// Thank you so much Wikipedia : https://en.wikipedia.org/wiki/Rotation_matrix
static void rotate_coord(double angle, double x_in, double y_in,
                         double* x_out, double* y_out)
{
	*x_out = (x_in * cos(angle)) - (y_in * sin(angle));
	*y_out = (x_in * sin(angle)) + (y_in * cos(angle));
}

// Tourne l'image de `angle` degrés dans le sens trigonométrique
Image rotation_image(Image image, double angle)
{
	double demi_hauteur_image = (double) image.hauteur / 2.;
	double demi_largeur_image = (double) image.largeur / 2.;

	double x_coin_sup_gauche;
	double y_coin_sup_gauche;
	rotate_coord(angle, -demi_largeur_image, demi_hauteur_image, &x_coin_sup_gauche, &y_coin_sup_gauche);
	
	double x_coin_sup_droit;
	double y_coin_sup_droit;
	rotate_coord(angle, demi_largeur_image, demi_hauteur_image, &x_coin_sup_droit, &y_coin_sup_droit);

	double demi_hauteur_nouvelle_image = fabs(y_coin_sup_gauche) > fabs(y_coin_sup_droit) ?  fabs(y_coin_sup_gauche) : fabs(y_coin_sup_droit);
	double demi_largeur_nouvelle_image = fabs(x_coin_sup_gauche) > fabs(x_coin_sup_droit) ?  fabs(x_coin_sup_gauche) : fabs(x_coin_sup_droit);

	printf("%f %f %f %f\n", demi_hauteur_image, demi_largeur_image, demi_hauteur_nouvelle_image, demi_largeur_nouvelle_image);

	UINT largeur_nouvelle_image = demi_largeur_nouvelle_image * 2.;
	UINT hauteur_nouvelle_image = demi_hauteur_nouvelle_image * 2.;

	Image rv = creer_image(largeur_nouvelle_image, hauteur_nouvelle_image);

	double x_centre_image_origine;
	double y_centre_image_origine;

	double x_centre_image_nouvelle;
	double y_centre_image_nouvelle;

	UINT x_coin_image_origine;
	UINT y_coin_image_origine;

	for (int i=1; i <= hauteur_nouvelle_image; i++) {
		for (int u=1; u <= largeur_nouvelle_image; u++) {
			x_centre_image_nouvelle = (double) u - demi_largeur_nouvelle_image;
			y_centre_image_nouvelle = (double) i - demi_hauteur_nouvelle_image;

			rotate_coord(-angle, x_centre_image_nouvelle, y_centre_image_nouvelle, &x_centre_image_origine, &y_centre_image_origine);

			x_coin_image_origine = (UINT) (demi_largeur_image + x_centre_image_origine);
			y_coin_image_origine = (UINT) (demi_hauteur_image + y_centre_image_origine);

			set_pixel_image(rv, u + 1, i - 1, get_pixel_image(image, x_coin_image_origine, y_coin_image_origine));
		}
	}
	
	return rv;
}
