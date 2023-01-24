/****************************************************************************** 
  Implementation du module image_pbm
******************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"types_macros.h"
#include"image.h"

/* macro donnant l'indice du pixel de coordonnees (_x,_y) de l'image _I 
   dans le tableau de pixels de l'image _I */
#define INDICE_PIXEL(_I,_x,_y) ((_x)-1)+(_I).la_largeur_de_l_image*((_y)-1)

/* creation d'une image PBM de dimensions L x H avec tous les pixels blancs */
Image creer_image(UINT L, UINT H)
{
	Image I;
	UINT i;
	
	I.la_largeur_de_l_image = L;
	I.la_hauteur_de_l_image = H;
	
	/* allocation dynamique d'un tableau de L*H Pixel*/
	I.pointeur_vers_le_tableau_de_pixels = (Pixel *)malloc(sizeof(Pixel)*L*H);
	
	/* test si le tableau a ete correctement alloue */
	if (I.pointeur_vers_le_tableau_de_pixels == (Pixel *)NULL)
	{
		ERREUR_FATALE("Impossible de creer une image");
	}
	
	/* remplir le tableau avec des pixels blancs */
	for (i=0; i<L*H; i++)
		I.pointeur_vers_le_tableau_de_pixels[i] = BLANC;
		
	return I;
}

/* suppression de l'image I = *p_I */
void supprimer_image(Image *p_I)
{
	free(p_I->pointeur_vers_le_tableau_de_pixels);
	p_I->la_largeur_de_l_image = 0;
	p_I->la_hauteur_de_l_image = 0;
}

/* renvoie la valeur du pixel (x,y) de l'image I
   si (x,y) est hors de l'image la fonction renvoie BLANC */
Pixel get_pixel_image(Image I, int x, int y)
{
	if (x<1 || x>I.la_largeur_de_l_image || y<1 || y>I.la_hauteur_de_l_image)
		return BLANC;
	return I.pointeur_vers_le_tableau_de_pixels[INDICE_PIXEL(I,x,y)];
}

/* change la valeur du pixel (x,y) de l'image I avec la valeur v
   si (x,y) est hors de l'image la fonction ne fait rien */
void set_pixel_image(Image I, int x, int y, Pixel v)
{
	if (x<1 || x>I.la_largeur_de_l_image || y<1 || y>I.la_hauteur_de_l_image)
		return;
	I.pointeur_vers_le_tableau_de_pixels[INDICE_PIXEL(I,x,y)] = v;
}

/* renvoie la largeur de l'image I */
UINT largeur_image(Image I)
{
	return I.la_largeur_de_l_image;
}

/* renvoie la hauteur de l'image I */
UINT hauteur_image(Image I)
{
	return I.la_hauteur_de_l_image;
}


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
void entete_fichier_pbm(FILE *f)
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
	
	/** PARTIE A COMPLETER **/
	
}

/* calculer l'image "negatif" de l'image I */
/* l'image I n'est pas modifiee et */
/* la fonction renvoie l'image "negatif" de I */
Image negatif_image(Image I)
{
	
	/** PARTIE A COMPLETER **/
	
}
