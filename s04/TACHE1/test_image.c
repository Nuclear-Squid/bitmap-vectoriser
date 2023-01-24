#include "image.h"

int main(int argc, char** argv) {
	if (argc < 2) ERREUR_FATALE("Nom de fichier image requis.\n");

	Image image = lire_fichier_image(argv[1]);
	Image negatif = negatif_image(image);

	ecrire_image(image);
	ecrire_image(negatif);
	
	supprimer_image(&image);
	supprimer_image(&negatif);

	return 0;
}
