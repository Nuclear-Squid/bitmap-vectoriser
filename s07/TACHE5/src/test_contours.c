#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "contours.h"
#include "image.h"

static void write_contour_data(FILE* out_file, const PointList* list) {
	ListNode* current_node = list->head;

	fprintf(out_file, "%u\n", list->len - 1);
	while (current_node) {
		fprintf(out_file, " %f %f\n", current_node->pos.x, current_node->pos.y);
		current_node = current_node->next;
	}
}

static u_int32_t sum_segements(const ContourList* list) {
	u_int32_t rv = 0;

	ContourListNode* current_node = list->head;
	while (current_node) {
		rv += current_node->contour->len - 1;
		current_node = current_node->next;
	}

	return rv;
}

static void write_all_contour_data(FILE* out_file, const ContourList* list) {
	ContourListNode* current_node = list->head;

	fprintf(out_file, "%u\n", list->len);
	while (current_node) {
		fprintf(out_file, "\n");
		write_contour_data(out_file, current_node->contour);
		current_node = current_node->next;
	}

	fprintf(out_file, "\ntotal segments: %u\n", sum_segements(list));
}

void show_help() {
	printf("Usage: ./test_contours <input_file> [style] <out_file>\n\n"
		"style :\n"
		"· stroke: (default) créé juste le contour de l’image.\n"
		"· fill: remplie tout à l’intérieur de l’image.\n");
}

// J’avais absolument aucune idée que c’était possible, c’est dégueulasse,
// mais c’est marrant donc je le laisse.
struct Args {
	char* in_file_name;
	FILE* out_file;
	RenderStyle style;
} arg_parse(int argc, char** argv) {
	if (argc > 4) {
		printf("Arguments invalides, lancez le programme avec `-h` pour plus d’infos\n");
		exit(1);
	}

	if (argc == 1 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
		show_help();
		exit(0);
	}

	if (argc == 2) {
		printf("Erreur: nom de fichier de sortie non précisé\n");
		exit(1);
	}

	struct Args rv = { 
		.in_file_name = argv[1],
		.style = Stroke,
	};

	if (argc == 3) {
		rv.out_file = fopen(argv[2], "w");
	}

	if (argc == 4) {
		if (strcmp(argv[2], "fill") == 0) {
			rv.style = Fill;
		} else if (strcmp(argv[2], "stroke") != 0) {
			printf("Attention: nom de style inconnu, utilisera le défaut: stroke\n");
		}

		rv.out_file = fopen(argv[3], "w");
	}

	return rv;
}

int main (int argc, char** argv) {
	struct Args args = arg_parse(argc, argv);

	Image image = lire_fichier_image(args.in_file_name);
	ContourList* contours = get_all_contours_image(&image, args.style);

	const bool print_node_data = false;
	if (print_node_data) { // Partie 1
		write_all_contour_data(args.out_file, contours);
	} else { // Partie 2
		serialise_contour_list(args.out_file, contours, image.hauteur, image.largeur);
	}

	delete_contour_list(contours);
	supprimer_image(&image);
	return 0;
}
