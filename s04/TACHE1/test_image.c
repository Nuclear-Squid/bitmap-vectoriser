#include "image.h"

int main(int argc, char** argv) {
	if (argc < 2) ERREUR_FATALE("Nom de fichier image requis.\n");

	Image image = lire_fichier_image(argv[1]);
	Image negatif = negatif_image(image);
	Image rotation_90_deg = rotation_90_deg_image(image);
	Image rotation = rotation_image(image, .79);

	ecrire_image(image);
	ecrire_image(negatif);
	ecrire_image(rotation_90_deg);
	ecrire_image(rotation);
	
	supprimer_image(&image);
	supprimer_image(&negatif);
	supprimer_image(&rotation_90_deg);
	supprimer_image(&rotation);

	return 0;
}
